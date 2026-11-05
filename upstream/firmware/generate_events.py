#!/usr/bin/env python3
import argparse
import os
import re
import sys

try:
    import yaml
except ImportError:
    print('PyYAML is required. Install with: python3 -m pip install pyyaml', file=sys.stderr)
    sys.exit(1)

EFFECT_MAP = {
    'hull': 'EV_EFFECT_HULL',
    'fuel': 'EV_EFFECT_FUEL',
    'credits': 'EV_EFFECT_CREDITS',
    'crew': 'EV_EFFECT_CREW',
    'cargo': 'EV_EFFECT_CARGO',
    'reputation': 'EV_EFFECT_REPUTATION',
    'xp': 'EV_EFFECT_XP',
    'upgrade': 'EV_EFFECT_UPGRADE',
    # frontier expansion
    'ore': 'EV_EFFECT_ORE',
    'gas': 'EV_EFFECT_GAS',
    'alloy': 'EV_EFFECT_ALLOY',
    'parts': 'EV_EFFECT_PARTS',
    'relic': 'EV_EFFECT_RELIC',
    'set_flag': 'EV_EFFECT_FLAG_SET',
    'clear_flag': 'EV_EFFECT_FLAG_CLEAR',
    'chain': 'EV_EFFECT_CHAIN',
    'he3': 'EV_EFFECT_HE3',
    'darkmatter': 'EV_EFFECT_DARK',
}
AT_MAP = {'empty': 0, 'poi': 1, 'outpost': 1, 'home': 2, 'station': 2, 'derelict': 3, 'wreck': 3}
FLAGS = {}   # name -> bit, filled from the YAML 'flags' lists


def flag_bit(name):
    name = str(name).strip().lower()
    if name not in FLAGS:
        if len(FLAGS) >= 32:
            raise ValueError('more than 32 story flags')
        FLAGS[name] = len(FLAGS)
    return FLAGS[name]


def flag_mask(names):
    if not names:
        return 0
    if isinstance(names, str):
        names = [names]
    m = 0
    for n in names:
        m |= 1 << flag_bit(n)
    return m

REQUIREMENT_MAP = {
    'pilot': 'EV_FLAG_REQUIRES_PILOT',
    'engineer': 'EV_FLAG_REQUIRES_ENGINEER',
    'scientist': 'EV_FLAG_REQUIRES_SCIENTIST',
    'soldier': 'EV_FLAG_REQUIRES_SOLDIER',
    'security': 'EV_FLAG_REQUIRES_SOLDIER',
    'medic': 'EV_FLAG_REQUIRES_MEDIC',
    'cargo': 'EV_FLAG_REQUIRES_CARGO',
    'credits': 'EV_FLAG_REQUIRES_CREDITS',
    'fuel': 'EV_FLAG_REQUIRES_FUEL',
}

MAX_OPTIONS = 4
MAX_OUTCOMES = 4
MAX_EFFECTS = 4


def c_string(value: str) -> str:
    if value is None:
        return 'NULL'
    # Replace common non-ASCII chars
    value = value.replace('—', '-').replace('–', '-').replace('…', '...').replace('\'', "'").replace('"', '"')
    # Keep only ASCII
    value = ''.join(c for c in value if ord(c) < 128)
    return '"' + (
        value.replace('\\', '\\\\')
        .replace('"', '\\"')
        .replace('\n', '\\n')
        .replace('\r', '\\r')
    ) + '"'




def parse_effect(effect_entry):
    if isinstance(effect_entry, dict):
        if len(effect_entry) != 1:
            raise ValueError(f'Effect entry must contain exactly one key/value: {effect_entry}')
        key, value = next(iter(effect_entry.items()))
        key_name = str(key).strip().lower()
        if key_name not in EFFECT_MAP:
            print(f'Warning: unknown effect key "{key}" will be skipped.', file=sys.stderr)
            return None
        if key_name in ('set_flag', 'clear_flag'):
            return EFFECT_MAP[key_name], flag_bit(value)
        return EFFECT_MAP[key_name], int(value)
    raise ValueError(f'Unsupported effect format: {effect_entry}')


def build_effects(effects):
    parsed = []
    if effects is None:
        print('Warning: effects list is None, returning empty list.', file=sys.stderr)
        effects = []
    for effect in effects:
        if len(parsed) >= MAX_EFFECTS:
            print(f'Warning: maximum number of effects ({MAX_EFFECTS}) reached, skipping remaining effects.', file=sys.stderr)
            break
        parsed_effect = parse_effect(effect)
        if parsed_effect is None:
            print(f'Warning: skipping invalid effect entry: {effect}', file=sys.stderr)
            continue
        parsed.append(parsed_effect)
    return parsed, len(parsed)


def build_outcome(outcome_data):
    name = outcome_data.get('name', '')
    chance = int(outcome_data.get('chance', 0))
    effects, count = build_effects(outcome_data.get('effects', []))
    return {
        'name': name,
        'chance': chance,
        'effect_count': min(count, MAX_EFFECTS),
        'effects': effects,
    }


def build_option(option_data):
    text = option_data.get('text', '')
    req_key = option_data.get('requirement', '').strip().lower()
    if req_key in REQUIREMENT_MAP:
        requirements = REQUIREMENT_MAP[req_key]
        required_count = option_data.get('requirement_count', 1)
    else:
        requirements = '0'
        required_count = 0
    outcomes_data = option_data.get('outcomes', []) or []
    outcomes = [build_outcome(o) for o in outcomes_data[:MAX_OUTCOMES]]
    return {
        'text': text,
        'requirements': requirements,
        'required_count': required_count,
        'outcome_count': len(outcomes_data[:MAX_OUTCOMES]),
        'outcomes': outcomes,
    }


def build_event(event_data):
    options_data = event_data.get('options', []) or []
    options = [build_option(o) for o in options_data[:MAX_OPTIONS]]
    at = str(event_data.get('at', 'empty')).strip().lower()
    if at not in AT_MAP:
        raise ValueError(f'unknown at: {at}')
    return {
        'id': int(event_data.get('id', 0)),
        'title': event_data.get('title', ''),
        'description': event_data.get('description', ''),
        'option_count': len(options_data[:MAX_OPTIONS]),
        'options': options,
        'req_flags': flag_mask(event_data.get('requires_flags')),
        'forbid_flags': flag_mask(event_data.get('forbids_flags')),
        'once': 1 if event_data.get('once') else 0,
        'at': AT_MAP[at],
        'weight': int(event_data.get('weight', 0)),
    }


def generate_c(events, output_path):
    lines = [
        '#include "eventbase.h"\n',
        'const event_t random_events[] = {',
    ]
    for event in events:
        lines.append('    {')
        lines.append(f'        .id = {event["id"]},')
        lines.append(f'        .title = {c_string(event["title"])},')
        lines.append(f'        .description = {c_string(event["description"])},')
        lines.append(f'        .option_count = {event["option_count"]},')
        lines.append('        .options = {')
        for option in event['options']:
            lines.append('            {')
            lines.append(f'                .description = {c_string(option["text"])},')
            lines.append(f'                .requirements = {option["requirements"]},')
            lines.append(f'                .required_count = {option["required_count"]},')
            lines.append(f'                .outcome_count = {option["outcome_count"]},')
            lines.append('                .outcomes = {')
            for outcome in option['outcomes']:
                lines.append('                    {')
                lines.append(f'                        .description = {c_string(outcome["name"])},')
                lines.append(f'                        .name = {c_string(outcome["name"])},')
                lines.append(f'                        .chance = {outcome["chance"]},')
                lines.append(f'                        .effect_count = {outcome["effect_count"]},')
                lines.append('                        .effects = {')
                for effect_type, effect_value in outcome['effects']:
                    lines.append('                            {')
                    lines.append(f'                                .type = {effect_type},')
                    lines.append(f'                                .value = {effect_value},')
                    lines.append('                            },')
                lines.append('                        },')
                lines.append('                    },')
            lines.append('                },')
            lines.append('            },')
        lines.append('        },')
        if event['req_flags'] or event['forbid_flags'] or event['once'] or event['at'] or event['weight']:
            lines.append(f'        .req_flags = 0x{event["req_flags"]:08x}u,')
            lines.append(f'        .forbid_flags = 0x{event["forbid_flags"]:08x}u,')
            lines.append(f'        .once = {event["once"]},')
            lines.append(f'        .at = {event["at"]},')
            lines.append(f'        .weight = {event["weight"]},')
        lines.append('    },')
    lines.append('};')
    lines.append(f'const int random_event_count = {len(events)};')

    with open(output_path, 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines) + '\n')


def main():
    parser = argparse.ArgumentParser(description='Generate C event data from events.yml.')
    parser.add_argument('input', nargs='?', default='events.yml', help='Path to events.yml')
    parser.add_argument('output', nargs='?', default='generated_events.c', help='Output C file')
    parser.add_argument('--pack', action='append', default=[], help='extra YAML pack(s) merged after the base events')
    args = parser.parse_args()

    all_events = []
    seen_ids = set()
    for path in [args.input] + args.pack:
        with open(path, 'r', encoding='utf-8') as f:
            data = yaml.safe_load(f)
        if not isinstance(data, dict) or 'events' not in data:
            raise ValueError(f'{path}: YAML file must contain a top-level "events" key.')
        for name in data.get('flags', []) or []:
            flag_bit(name)
        for e in data['events']:
            if int(e.get('id', 0)) in seen_ids:
                raise ValueError(f'{path}: duplicate event id {e.get("id")}')
            seen_ids.add(int(e.get('id', 0)))
        all_events.extend(data['events'])
        print(f'Loaded {len(data["events"])} events from {path}.')
    if len(all_events) > 100:
        raise ValueError('the engine shuffles at most 100 events (shuffled_events[100])')
    events = [build_event(e) for e in all_events]
    generate_c(events, args.output)
    if FLAGS:
        print('Story flags: ' + ', '.join(f'{k}={v}' for k, v in FLAGS.items()))
    print(f'Generated {args.output} with {len(events)} events.')


if __name__ == '__main__':
    main()
