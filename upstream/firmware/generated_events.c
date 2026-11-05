#include "eventbase.h"

const event_t random_events[] = {
    {
        .id = 1,
        .title = "Galactic Customs Authority",
        .description = "A gleaming authority cruiser looms across your path, its polished hull reflecting distant stars. A tinny radio crackles: 'Merchant vessel, prepare for cargo inspection. We've flagged irregularities in your manifest.' The customs officers await your response.",
        .option_count = 4,
        .options = {
            {
                .description = "Comply fully",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "The officers nod approvingly and clear you for departure. Your professionalism earns their respect.",
                        .name = "The officers nod approvingly and clear you for departure. Your professionalism earns their respect.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "Their scanners find discrepancies in your documentation. Valuable cargo gets impounded.",
                        .name = "Their scanners find discrepancies in your documentation. Valuable cargo gets impounded.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -5,
                            },
                        },
                    },
                },
            },
            {
                .description = "Bribe the officer",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "The bribe works perfectly. The lead officer pockets it discreetly and waves you through.",
                        .name = "The bribe works perfectly. The lead officer pockets it discreetly and waves you through.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -50,
                            },
                        },
                    },
                    {
                        .description = "Galactic Review was monitoring. You're caught red-handed and face severe penalties.",
                        .name = "Galactic Review was monitoring. You're caught red-handed and face severe penalties.",
                        .chance = 40,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -10,
                            },
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -200,
                            },
                        },
                    },
                },
            },
            {
                .description = "Hide contraband",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your contraband stays hidden. The inspection finds nothing suspicious.",
                        .name = "Your contraband stays hidden. The inspection finds nothing suspicious.",
                        .chance = 70,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 50,
                            },
                        },
                    },
                    {
                        .description = "A routine secondary scan exposes your illegal goods. Confiscation is swift and total.",
                        .name = "A routine secondary scan exposes your illegal goods. Confiscation is swift and total.",
                        .chance = 30,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -8,
                            },
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -100,
                            },
                        },
                    },
                },
            },
            {
                .description = "Forge documents (Scientist)",
                .requirements = EV_FLAG_REQUIRES_SCIENTIST,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your scientist's forgery is masterful-flawless enough to survive the ship's math-machine verification.",
                        .name = "Your scientist's forgery is masterful-flawless enough to survive the ship's math-machine verification.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "The patrol's analysis exposes the fake documents immediately. Tribunal trouble ensues.",
                        .name = "The patrol's analysis exposes the fake documents immediately. Tribunal trouble ensues.",
                        .chance = 20,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -5,
                            },
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -500,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 2,
        .title = "Graveyard of Lost Starships",
        .description = "Your sensors light up with thousands of contacts. Before you lies a massive debris field-the scattered remains of an ancient space battle. Jagged hull plating and twisted metal drift silently like tombstones. Your navigation computer urges caution as you approach.",
        .option_count = 4,
        .options = {
            {
                .description = "Attempt manual maneuvering",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your pilot maneuvers like a rocket jockey on a golden-age flyer. The crew gains valuable experience.",
                        .name = "Your pilot maneuvers like a rocket jockey on a golden-age flyer. The crew gains valuable experience.",
                        .chance = 75,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 50,
                            },
                        },
                    },
                    {
                        .description = "A hidden piece of debris rips into your starboard hull. The damage is substantial.",
                        .name = "A hidden piece of debris rips into your starboard hull. The damage is substantial.",
                        .chance = 25,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Travel around",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your course adjustment takes you safely around the wreckage.",
                        .name = "Your course adjustment takes you safely around the wreckage.",
                        .chance = 50,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 20,
                            },
                        },
                    },
                    {
                        .description = "The detour burns precious fuel as you work around the wreckage.",
                        .name = "The detour burns precious fuel as you work around the wreckage.",
                        .chance = 50,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Call for help",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "A nearby pilot group intercepts your call and clears a path. You owe them a favor.",
                        .name = "A nearby pilot group intercepts your call and clears a path. You owe them a favor.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "They demand payment for the help. You pay up, but it hurts your bottom line.",
                        .name = "They demand payment for the help. You pay up, but it hurts your bottom line.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -250,
                            },
                        },
                    },
                },
            },
            {
                .description = "Advanced navigation (Pilot)",
                .requirements = EV_FLAG_REQUIRES_PILOT,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your pilot's expertise cuts through the debris field like a knife. The crew is astounded.",
                        .name = "Your pilot's expertise cuts through the debris field like a knife. The crew is astounded.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 50,
                            },
                        },
                    },
                    {
                        .description = "A grazing impact marks your hull, but mostly you make it through unscathed.",
                        .name = "A grazing impact marks your hull, but mostly you make it through unscathed.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 4,
        .title = "Raiders from the Crimson Nebula",
        .description = "Three sleek cutters emerge from the radiation haze of the Crimson Nebula, cutting off your escape vector. Their ray cannons gleam in the starlight. A gravelly voice breaks through your comms: 'Stand down, merchant. Your cargo is confiscated.' Pirate raiders have found you.",
        .option_count = 4,
        .options = {
            {
                .description = "Fight back",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your crew fights with desperate courage. The pirates flee, leaving their plunder behind.",
                        .name = "Your crew fights with desperate courage. The pirates flee, leaving their plunder behind.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = 300,
                            },
                        },
                    },
                    {
                        .description = "The engagement is fierce. Your hull takes punishing hits before they withdraw.",
                        .name = "The engagement is fierce. Your hull takes punishing hits before they withdraw.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Flee",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your engines roar to life. You punch through the pirate blockade and disappear into hyperspace.",
                        .name = "Your engines roar to life. You punch through the pirate blockade and disappear into hyperspace.",
                        .chance = 70,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 50,
                            },
                        },
                    },
                    {
                        .description = "They cut off your escape route. Cargo jettison is your only option to gain speed.",
                        .name = "They cut off your escape route. Cargo jettison is your only option to gain speed.",
                        .chance = 30,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -16,
                            },
                        },
                    },
                },
            },
            {
                .description = "Negotiate",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Cunning words and a small tribute satisfy the raiders. They let you pass.",
                        .name = "Cunning words and a small tribute satisfy the raiders. They let you pass.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -5,
                            },
                        },
                    },
                    {
                        .description = "Their demands escalate. You're forced to surrender more than you offered.",
                        .name = "Their demands escalate. You're forced to surrender more than you offered.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -8,
                            },
                        },
                    },
                },
            },
            {
                .description = "Security response (Security)",
                .requirements = EV_FLAG_REQUIRES_SOLDIER,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your security team repels the attack before it even starts. The pirates retreat in shame.",
                        .name = "Your security team repels the attack before it even starts. The pirates retreat in shame.",
                        .chance = 80,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = 8,
                            },
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "The brief firefight is over quickly, but your hull bears the scars.",
                        .name = "The brief firefight is over quickly, but your hull bears the scars.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 5,
        .title = "The Obsidian Monolith",
        .description = "Deep in a forgotten asteroid field, your salvage scanner detects something: a perfectly geometric obsidian structure, pulsing with an eerie neon glow. This ancient alien artifact hums like a forgotten rocket-age dream, predating any known civilization. Its origins remain a complete mystery.",
        .option_count = 4,
        .options = {
            {
                .description = "Sell it",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "A retro collectors' market hungers for alien relics. You pocket a tidy sum.",
                        .name = "A retro collectors' market hungers for alien relics. You pocket a tidy sum.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = 800,
                            },
                        },
                    },
                    {
                        .description = "You sell it cheaply, realizing too late it was far more valuable intact.",
                        .name = "You sell it cheaply, realizing too late it was far more valuable intact.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = -10,
                            },
                        },
                    },
                },
            },
            {
                .description = "Ignore it",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Leaving it behind feels safe, and you continue on your way without incident.",
                        .name = "Leaving it behind feels safe, and you continue on your way without incident.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 10,
                            },
                        },
                    },
                    {
                        .description = "Later reports suggest the artifact held technology worth fortunes. You missed it.",
                        .name = "Later reports suggest the artifact held technology worth fortunes. You missed it.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = -10,
                            },
                        },
                    },
                },
            },
            {
                .description = "Dissect it (Medic)",
                .requirements = EV_FLAG_REQUIRES_MEDIC,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your medic carefully disassembles the monolith. The components reveal a way to harness biological power safely. A breakthrough!",
                        .name = "Your medic carefully disassembles the monolith. The components reveal a way to harness biological power safely. A breakthrough!",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_UPGRADE,
                                .value = 1,
                            },
                        },
                    },
                    {
                        .description = "The artifact destabilizes during disassembly. A violent energy surge damages your ship.",
                        .name = "The artifact destabilizes during disassembly. A violent energy surge damages your ship.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Study it (Scientist)",
                .requirements = EV_FLAG_REQUIRES_SCIENTIST,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your scientist's analysis reveals a way to harness its power safely. A breakthrough!",
                        .name = "Your scientist's analysis reveals a way to harness its power safely. A breakthrough!",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_UPGRADE,
                                .value = 1,
                            },
                        },
                    },
                    {
                        .description = "As they study it, the artifact destabilizes violently. Energy floods the hold.",
                        .name = "As they study it, the artifact destabilizes violently. Energy floods the hold.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 6,
        .title = "The Dying Freighter",
        .description = "A distress beacon pulls your attention toward a crippled deep-space freighter, its reactor stuttering dangerously. Through the viewscreen, you see a damaged hull and desperate crew signals. The captain's voice comes through, barely a whisper: 'Please... our rocket tanks are dry. Can anyone spare a puff of fuel?'",
        .option_count = 3,
        .options = {
            {
                .description = "Sell them 3 fuel",
                .requirements = EV_FLAG_REQUIRES_FUEL,
                .required_count = 3,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "A solid deal. You trade 3 units of fuel for 15 credits. Both sides profit.",
                        .name = "A solid deal. You trade 3 units of fuel for 15 credits. Both sides profit.",
                        .chance = 60,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -3,
                            },
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = 150,
                            },
                        },
                    },
                    {
                        .description = "They're so desperate they offer 300 credits. Easy money.",
                        .name = "They're so desperate they offer 300 credits. Easy money.",
                        .chance = 40,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -3,
                            },
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = 300,
                            },
                        },
                    },
                },
            },
            {
                .description = "Give them 3 Fuel",
                .requirements = EV_FLAG_REQUIRES_FUEL,
                .required_count = 3,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Grateful and relieved, they load your hold with salvage and rare materials.",
                        .name = "Grateful and relieved, they load your hold with salvage and rare materials.",
                        .chance = 70,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -3,
                            },
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = 10,
                            },
                        },
                    },
                    {
                        .description = "They're grateful for your mercy, but can only offer thanks and well-wishes.",
                        .name = "They're grateful for your mercy, but can only offer thanks and well-wishes.",
                        .chance = 30,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -3,
                            },
                            {
                                .type = EV_EFFECT_XP,
                                .value = 200,
                            },
                        },
                    },
                },
            },
            {
                .description = "Leave them, you can't spare the fuel",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You accelerate away, but they make no hostile move. Just desperate silence.",
                        .name = "You accelerate away, but they make no hostile move. Just desperate silence.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "As you jump to hyperspace, they fire on you in desperation. You're struck hard.",
                        .name = "As you jump to hyperspace, they fire on you in desperation. You're struck hard.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 7,
        .title = "Mutiny Brewing Below Decks",
        .description = "Tension explodes between your crew members in the commons bay. Your navigator and cargo master are locked in heated debate about mission priorities. Voices rise. Fists clench. If you don't intervene soon, this could tear the crew apart.",
        .option_count = 4,
        .options = {
            {
                .description = "Mediate",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your calm mediation brings the crew to compromise. Unity restored and morale boosted.",
                        .name = "Your calm mediation brings the crew to compromise. Unity restored and morale boosted.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 200,
                            },
                        },
                    },
                    {
                        .description = "The debate drags on, but eventually tempers cool and the crew reaches compromise.",
                        .name = "The debate drags on, but eventually tempers cool and the crew reaches compromise.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                },
            },
            {
                .description = "Ignore",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You maintain course. The argument fades as priorities shift. Nothing breaks.",
                        .name = "You maintain course. The argument fades as priorities shift. Nothing breaks.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 200,
                            },
                        },
                    },
                    {
                        .description = "The dispute festers. Resentment spreads through the crew like poison.",
                        .name = "The dispute festers. Resentment spreads through the crew like poison.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = -200,
                            },
                        },
                    },
                },
            },
            {
                .description = "Pick a side",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "One side feels vindicated. The issue is settled, though some harbor quiet resentment.",
                        .name = "One side feels vindicated. The issue is settled, though some harbor quiet resentment.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 200,
                            },
                        },
                    },
                    {
                        .description = "Those on the losing side feel slighted. Friction remains below the surface.",
                        .name = "Those on the losing side feel slighted. Friction remains below the surface.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = -200,
                            },
                        },
                    },
                },
            },
            {
                .description = "Counsel crew (Medic)",
                .requirements = EV_FLAG_REQUIRES_MEDIC,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your medic's counseling brings clarity. The crew's spirits soar with newfound purpose.",
                        .name = "Your medic's counseling brings clarity. The crew's spirits soar with newfound purpose.",
                        .chance = 70,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 400,
                            },
                        },
                    },
                    {
                        .description = "The session is intense and ultimately strengthens the crew's unity.",
                        .name = "The session is intense and ultimately strengthens the crew's unity.",
                        .chance = 30,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 200,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 8,
        .title = "Bio-Hazard Alert",
        .description = "A routine scan triggers your ship's alarm system. Biohazard readings spike in the cargo bay-something in one of your containers has become contaminated with an unknown, glowing pathogen. The cargo holds 80% of your profits for this run. What do you do?",
        .option_count = 4,
        .options = {
            {
                .description = "Dispose of it",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You make the hard call. The contaminated cargo is spaced. Risk averted.",
                        .name = "You make the hard call. The contaminated cargo is spaced. Risk averted.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -2,
                            },
                        },
                    },
                    {
                        .description = "Attempting disposal causes a catastrophic vent. Most of your cargo is lost in the process.",
                        .name = "Attempting disposal causes a catastrophic vent. Most of your cargo is lost in the process.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -16,
                            },
                        },
                    },
                },
            },
            {
                .description = "Sell it anyway",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You sell it anyway. The buyers are dazzled by the glow. Profit secured.",
                        .name = "You sell it anyway. The buyers are dazzled by the glow. Profit secured.",
                        .chance = 60,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = 500,
                            },
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "Word spreads. Your reputation for cutting corners is now known across the sector.",
                        .name = "Word spreads. Your reputation for cutting corners is now known across the sector.",
                        .chance = 40,
                        .effect_count = 3,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = -100,
                            },
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = 200,
                            },
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Quarantine it",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Isolation and careful handling preserve most of your cargo. A smart call pays off.",
                        .name = "Isolation and careful handling preserve most of your cargo. A smart call pays off.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -6,
                            },
                        },
                    },
                    {
                        .description = "Quarantine procedures are difficult but ultimately effective.",
                        .name = "Quarantine procedures are difficult but ultimately effective.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 200,
                            },
                        },
                    },
                },
            },
            {
                .description = "Analyze contamination (Scientist)",
                .requirements = EV_FLAG_REQUIRES_SCIENTIST,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your scientist neutralizes the pathogen completely. The cargo is salvaged and safe.",
                        .name = "Your scientist neutralizes the pathogen completely. The cargo is salvaged and safe.",
                        .chance = 70,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -1,
                            },
                            {
                                .type = EV_EFFECT_XP,
                                .value = 200,
                            },
                        },
                    },
                    {
                        .description = "The analysis shows severe corruption. A portions must still be destroyed.",
                        .name = "The analysis shows severe corruption. A portions must still be destroyed.",
                        .chance = 30,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -8,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 9,
        .title = "Solar Fury Descends",
        .description = "The peaceful starfield erupts in chaos. A massive solar flare tears from the sun, invisible waves of radiation and charged particles racing toward your vessel like an atomic sunburst. Your alarms wail as the storm engulfs your trajectory. Only your quick thinking can save the ship now.",
        .option_count = 4,
        .options = {
            {
                .description = "Wait it out",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You hold your breath and wait. The storm passes harmlessly around you.",
                        .name = "You hold your breath and wait. The storm passes harmlessly around you.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 200,
                            },
                        },
                    },
                    {
                        .description = "Patience pays off, but your route options narrow.",
                        .name = "Patience pays off, but your route options narrow.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Push through",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your engines burn bright as you punch through. Speed overcomes the radiation waves.",
                        .name = "Your engines burn bright as you punch through. Speed overcomes the radiation waves.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "You made the wrong call. The solar wind batters your hull with violent fury.",
                        .name = "You made the wrong call. The solar wind batters your hull with violent fury.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Secure cargo",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You carefully redistribute weight and secure everything. The cargo holds steady through the storm.",
                        .name = "You carefully redistribute weight and secure everything. The cargo holds steady through the storm.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "Despite your efforts, the ship pitches violently. Progress slows to a crawl.",
                        .name = "Despite your efforts, the ship pitches violently. Progress slows to a crawl.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -5,
                            },
                        },
                    },
                },
            },
            {
                .description = "Stabilize systems (Engineer)",
                .requirements = EV_FLAG_REQUIRES_ENGINEER,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your engineer makes surgical adjustments to hull joints. You ride out the storm unscathed.",
                        .name = "Your engineer makes surgical adjustments to hull joints. You ride out the storm unscathed.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = 1,
                            },
                        },
                    },
                    {
                        .description = "The systems strain under the assault. They hold, barely, but not without stress.",
                        .name = "The systems strain under the assault. They hold, barely, but not without stress.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 10,
        .title = "Dysentery Outbreak",
        .description = "As you glide through a treacherous asteroid belt, contaminated water from a cracked recycler spreads through the crew. Symptoms of dysentery begin to manifest-fever, dehydration, and weakness. Even the old frontier diseases have found their way to the stars.",
        .option_count = 3,
        .options = {
            {
                .description = "Rest and recover",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Most crew recover with rest, but one succumbs to the relentless dysentery.",
                        .name = "Most crew recover with rest, but one succumbs to the relentless dysentery.",
                        .chance = 70,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The outbreak worsens during rest. Multiple crew members are lost to the illness.",
                        .name = "The outbreak worsens during rest. Multiple crew members are lost to the illness.",
                        .chance = 30,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Push through",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You continue despite the illness. One crew member collapses and doesn't recover.",
                        .name = "You continue despite the illness. One crew member collapses and doesn't recover.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The strain proves fatal. Dysentery claims two lives before you can stop it.",
                        .name = "The strain proves fatal. Dysentery claims two lives before you can stop it.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Treat with medicine (Medic)",
                .requirements = EV_FLAG_REQUIRES_MEDIC,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your medic administers effective treatment. The outbreak is contained with minimal loss.",
                        .name = "Your medic administers effective treatment. The outbreak is contained with minimal loss.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 200,
                            },
                        },
                    },
                    {
                        .description = "Treatment helps, but one crew member still falls to the disease.",
                        .name = "Treatment helps, but one crew member still falls to the disease.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 13,
        .title = "The Crystalline Trader",
        .description = "A vessel of impossible geometry shimmers into view-a ship that shouldn't be able to exist within normal spacetime. Its pilot, a being of pulsing crystalline light, makes contact: 'I have goods no other civilization possesses. Interested in trade... friend?'",
        .option_count = 3,
        .options = {
            {
                .description = "Accept trade",
                .requirements = EV_FLAG_REQUIRES_CREDITS,
                .required_count = 1000,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "The crystal being smiles-if such a thing is possible. The trade brings mutual benefit and profit.",
                        .name = "The crystal being smiles-if such a thing is possible. The trade brings mutual benefit and profit.",
                        .chance = 60,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = 8,
                            },
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -1000,
                            },
                        },
                    },
                    {
                        .description = "The deal seems legitimate. Your instincts say something is off, but you accept anyway.",
                        .name = "The deal seems legitimate. Your instincts say something is off, but you accept anyway.",
                        .chance = 40,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = 4,
                            },
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -1000,
                            },
                        },
                    },
                },
            },
            {
                .description = "Decline",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You politely refuse. No complications or entanglements follow.",
                        .name = "You politely refuse. No complications or entanglements follow.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "You decline, but the offer haunts you. What if you'd taken it?",
                        .name = "You decline, but the offer haunts you. What if you'd taken it?",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = -100,
                            },
                        },
                    },
                },
            },
            {
                .description = "Negotiate",
                .requirements = EV_FLAG_REQUIRES_CREDITS,
                .required_count = 800,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your counteroffers are sharp. The merchant relents and gives you an even better price.",
                        .name = "Your counteroffers are sharp. The merchant relents and gives you an even better price.",
                        .chance = 60,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = 8,
                            },
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -800,
                            },
                        },
                    },
                    {
                        .description = "They refuse to budge further. You take their original offer or walk away empty-handed.",
                        .name = "They refuse to budge further. You take their original offer or walk away empty-handed.",
                        .chance = 40,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = 4,
                            },
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -800,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 14,
        .title = "The Faulty Star Charts",
        .description = "Your navigation console flickers with a warning. The maps you're using-supposedly updated just weeks ago-contain significant errors. A critical waypoint listed on your charts doesn't exist. Other routes suddenly lead nowhere. Your trajectory is compromised.",
        .option_count = 3,
        .options = {
            {
                .description = "Proceed anyway",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Luck favors the bold. You navigate the corrected course and save significant effort.",
                        .name = "Luck favors the bold. You navigate the corrected course and save significant effort.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "The maps lead you astray. You waste precious fuel correcting your course.",
                        .name = "The maps lead you astray. You waste precious fuel correcting your course.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Recalculate",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your careful recalculations yield a safe, indirect route that ensures accuracy.",
                        .name = "Your careful recalculations yield a safe, indirect route that ensures accuracy.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "The detour is costly, but you're confident in the path now.",
                        .name = "The detour is costly, but you're confident in the path now.",
                        .chance = 40,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = -100,
                            },
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Pilot correction (Pilot)",
                .requirements = EV_FLAG_REQUIRES_PILOT,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your pilot's instincts are flawless. They chart a shortcut that saves fuel and resources alike.",
                        .name = "Your pilot's instincts are flawless. They chart a shortcut that saves fuel and resources alike.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = 1,
                            },
                        },
                    },
                    {
                        .description = "Even with the pilot's skill, stellar drift causes minor course corrections and fuel burn.",
                        .name = "Even with the pilot's skill, stellar drift causes minor course corrections and fuel burn.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 16,
        .title = "Shadows in the Docking Bay",
        .description = "A hooded figure approaches you in the darkened docking bay, glancing over their shoulder nervously. 'I know what you carry. I know what you want to carry. I can get you goods-valuable, illegal, off-the-books goods. No questions asked. Interested?' The offer hangs dangerous and tantalizing.",
        .option_count = 3,
        .options = {
            {
                .description = "Buy",
                .requirements = EV_FLAG_REQUIRES_CREDITS,
                .required_count = 1000,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "The illegal goods are legitimate treasure. Your hold suddenly becomes far more valuable.",
                        .name = "The illegal goods are legitimate treasure. Your hold suddenly becomes far more valuable.",
                        .chance = 60,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -1000,
                            },
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = 10,
                            },
                        },
                    },
                    {
                        .description = "The goods turn out to be low-quality forgeries. You wasted money on garbage.",
                        .name = "The goods turn out to be low-quality forgeries. You wasted money on garbage.",
                        .chance = 40,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -1000,
                            },
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = 3,
                            },
                        },
                    },
                },
            },
            {
                .description = "Ignore",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 1,
                .outcomes = {
                    {
                        .description = "Wisdom prevails. You pass on the deal and sleep well that night.",
                        .name = "Wisdom prevails. You pass on the deal and sleep well that night.",
                        .chance = 100,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                },
            },
            {
                .description = "Report",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Reporting the smuggler earns you a tidy bounty from the authorities.",
                        .name = "Reporting the smuggler earns you a tidy bounty from the authorities.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = 500,
                            },
                        },
                    },
                    {
                        .description = "The paperwork drags on. Your schedule crumbles.",
                        .name = "The paperwork drags on. Your schedule crumbles.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = -100,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 17,
        .title = "The Orphaned Probe",
        .description = "Drifting lazily across your scanner is an abandoned drone, its transponder still weakly broadcasting. It's heavily damaged but-if you had to guess-still functional. Someone lost this out here. The question is: who, and is it worth recovering?",
        .option_count = 3,
        .options = {
            {
                .description = "Recover it",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your salvage scanner identifies valuable components. This find is worth credits and cargo space.",
                        .name = "Your salvage scanner identifies valuable components. This find is worth credits and cargo space.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = 2,
                            },
                        },
                    },
                    {
                        .description = "It looks valuable but turns out to be corroded scrap. A waste of effort.",
                        .name = "It looks valuable but turns out to be corroded scrap. A waste of effort.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = -100,
                            },
                        },
                    },
                },
            },
            {
                .description = "Ignore",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 1,
                .outcomes = {
                    {
                        .description = "You leave it behind without second thought. Peace prevails.",
                        .name = "You leave it behind without second thought. Peace prevails.",
                        .chance = 100,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                },
            },
            {
                .description = "Repair it (Engineer)",
                .requirements = EV_FLAG_REQUIRES_ENGINEER,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "The drone's owner makes contact via hidden systems. A generous reward is transferred immediately.",
                        .name = "The drone's owner makes contact via hidden systems. A generous reward is transferred immediately.",
                        .chance = 50,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = 500,
                            },
                        },
                    },
                    {
                        .description = "The drone is an advanced repair unit. Upon activation, it strengthens your hull integrity.",
                        .name = "The drone is an advanced repair unit. Upon activation, it strengthens your hull integrity.",
                        .chance = 50,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = 1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 18,
        .title = "The Glowing Plague",
        .description = "Geiger counters erupt in a cacophony of clicks and whines. Your cargo bay is bathed in hazardous radiation-something in your shipment has broken containment. The crew runs for protective gear while your systems sound increasingly frantic warnings. Time is not your friend here.",
        .option_count = 4,
        .options = {
            {
                .description = "Vent bay",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You blow the airlock. Radiation vents into space and the threat vanishes.",
                        .name = "You blow the airlock. Radiation vents into space and the threat vanishes.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -3,
                            },
                        },
                    },
                    {
                        .description = "The vent process is slow and complicated. Radiation lingers longer than expected.",
                        .name = "The vent process is slow and complicated. Radiation lingers longer than expected.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -5,
                            },
                        },
                    },
                },
            },
            {
                .description = "Seal it",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You seal the bay tight. The radiation is contained, though it lingers behind closed doors.",
                        .name = "You seal the bay tight. The radiation is contained, though it lingers behind closed doors.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -3,
                            },
                        },
                    },
                    {
                        .description = "The seal holds... for now. But stored energy could rupture it anytime.",
                        .name = "The seal holds... for now. But stored energy could rupture it anytime.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -4,
                            },
                        },
                    },
                },
            },
            {
                .description = "Ignore",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "The radiation dissipates naturally. No further incident occurs.",
                        .name = "The radiation dissipates naturally. No further incident occurs.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 200,
                            },
                        },
                    },
                    {
                        .description = "Crew members begin showing radiation sickness. One falls critically ill.",
                        .name = "Crew members begin showing radiation sickness. One falls critically ill.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Treat exposure (Medic)",
                .requirements = EV_FLAG_REQUIRES_MEDIC,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your medic administers preventative treatment. The crew recovers without lasting harm.",
                        .name = "Your medic administers preventative treatment. The crew recovers without lasting harm.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 200,
                            },
                        },
                    },
                    {
                        .description = "The treatment process is difficult and only partially succeeds. No casualties, but the crew is weakened.",
                        .name = "The treatment process is difficult and only partially succeeds. No casualties, but the crew is weakened.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 19,
        .title = "Sparks in the Engine Room",
        .description = "A shower of sparks erupts from the engine compartment. Your second engineer limps out, blackened and shaken. 'Captain, we've got a critical failure in the main turbine assembly. It's holding for now, barely, but if it gives way completely...'",
        .option_count = 4,
        .options = {
            {
                .description = "Repair",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your engineers work through the night. By dawn, the component is fully restored.",
                        .name = "Your engineers work through the night. By dawn, the component is fully restored.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = 1,
                            },
                        },
                    },
                    {
                        .description = "Repairs take longer than anticipated. Progress stalls while you work.",
                        .name = "Repairs take longer than anticipated. Progress stalls while you work.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Replace",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "The replacement part installs perfectly and performs better than the original.",
                        .name = "The replacement part installs perfectly and performs better than the original.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = 1,
                            },
                        },
                    },
                    {
                        .description = "The new component is expensive. Your emergency credits take a hit.",
                        .name = "The new component is expensive. Your emergency credits take a hit.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -500,
                            },
                        },
                    },
                },
            },
            {
                .description = "Ignore",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "The component holds for now. You maintain speed and pressure as if nothing is wrong.",
                        .name = "The component holds for now. You maintain speed and pressure as if nothing is wrong.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = 1,
                            },
                        },
                    },
                    {
                        .description = "The failure is postponed, not prevented. Viloent vibrations and power fluctuations plague your systems.",
                        .name = "The failure is postponed, not prevented. Viloent vibrations and power fluctuations plague your systems.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -3,
                            },
                        },
                    },
                },
            },
            {
                .description = "Optimize fix (Engineer)",
                .requirements = EV_FLAG_REQUIRES_ENGINEER,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your engineer tweaks the repairs to achieve better-than-new performance. ",
                        .name = "Your engineer tweaks the repairs to achieve better-than-new performance. ",
                        .chance = 60,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = 1,
                            },
                            {
                                .type = EV_EFFECT_HULL,
                                .value = 1,
                            },
                        },
                    },
                    {
                        .description = "The optimization process is complex. Some gains are realized, but not all.",
                        .name = "The optimization process is complex. Some gains are realized, but not all.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = 1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 20,
        .title = "Space Plague Strikes",
        .description = "Your best navigator collapses on the bridge. Fever. Chills. An illness spreading through your ranks with disturbing speed. The medical officer looks grim: 'Captain, I've never seen anything like this before. Whatever this is, it could incapacitate the crew.'",
        .option_count = 3,
        .options = {
            {
                .description = "Rest",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Rest works wonders. Your crew member returns to full duty.",
                        .name = "Rest works wonders. Your crew member returns to full duty.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 50,
                            },
                        },
                    },
                    {
                        .description = "Recovery is slower than hoped. You're forced to maintain reduced operational tempo.",
                        .name = "Recovery is slower than hoped. You're forced to maintain reduced operational tempo.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = -50,
                            },
                        },
                    },
                },
            },
            {
                .description = "Push on",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your crew member pushes through. They're determined to keep working despite the illness.",
                        .name = "Your crew member pushes through. They're determined to keep working despite the illness.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 50,
                            },
                        },
                    },
                    {
                        .description = "The illness worsens dramatically. Your crew member becomes bedridden and useless.",
                        .name = "The illness worsens dramatically. Your crew member becomes bedridden and useless.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Treat illness (Medic)",
                .requirements = EV_FLAG_REQUIRES_MEDIC,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your medic prescribes the perfect treatment. Recovery is swift and complete.",
                        .name = "Your medic prescribes the perfect treatment. Recovery is swift and complete.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "The treatment helps, but the recovery process still demands careful observation.",
                        .name = "The treatment helps, but the recovery process still demands careful observation.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 50,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 21,
        .title = "Fortune in the Deep",
        .description = "While exploring a dead moon's caverns, your survey team falls through a concealed floor into an ancient vault. Cases upon cases of pristine, pre-war goods sit untouched for centuries. Whoever hid this treasure left no explanation-just riches waiting in the darkness.",
        .option_count = 4,
        .options = {
            {
                .description = "Take all",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your team loads everything. Riches beyond measure now fill your hold.",
                        .name = "Your team loads everything. Riches beyond measure now fill your hold.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = 10,
                            },
                        },
                    },
                    {
                        .description = "The vault collapses as you grab the last case. The concussion damages your ship.",
                        .name = "The vault collapses as you grab the last case. The concussion damages your ship.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Take some",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You fill several crates with the most valuable items and depart cautiously.",
                        .name = "You fill several crates with the most valuable items and depart cautiously.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = 5,
                            },
                        },
                    },
                    {
                        .description = "You could have carried more if you'd been bolder. Hindsight is cruel.",
                        .name = "You could have carried more if you'd been bolder. Hindsight is cruel.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = 2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Leave it",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You walk away unharmed. Caution was the right call this time.",
                        .name = "You walk away unharmed. Caution was the right call this time.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "You leave empty-handed, wondering what fortune you just passed by.",
                        .name = "You leave empty-handed, wondering what fortune you just passed by.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = -100,
                            },
                        },
                    },
                },
            },
            {
                .description = "Scan first (Scientist)",
                .requirements = EV_FLAG_REQUIRES_SCIENTIST,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "The scan reveals no booby traps or dangers. You collect everything safely.",
                        .name = "The scan reveals no booby traps or dangers. You collect everything safely.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = 10,
                            },
                        },
                    },
                    {
                        .description = "The scan detects some hazards, but not all. You proceed with caution and partial success.",
                        .name = "The scan detects some hazards, but not all. You proceed with caution and partial success.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = 5,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 22,
        .title = "Giant Spider Infestation",
        .description = "Boarding a derelict freighter for salvage, your team discovers a nightmare straight out of a pulp serial: massive, genetically engineered spiders that have overrun the ship. Their webs clog corridors, and their venomous bites threaten your crew.",
        .option_count = 3,
        .options = {
            {
                .description = "Fight them off",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your crew battles fiercely and clears the spiders, but one member falls to venomous bites.",
                        .name = "Your crew battles fiercely and clears the spiders, but one member falls to venomous bites.",
                        .chance = 70,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = 5,
                            },
                        },
                    },
                    {
                        .description = "The spiders overwhelm your team. Multiple crew are lost in the arachnid assault.",
                        .name = "The spiders overwhelm your team. Multiple crew are lost in the arachnid assault.",
                        .chance = 30,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Seal and abandon",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 1,
                .outcomes = {
                    {
                        .description = "You seal the derelict and retreat safely, losing the salvage opportunity.",
                        .name = "You seal the derelict and retreat safely, losing the salvage opportunity.",
                        .chance = 100,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = -50,
                            },
                        },
                    },
                },
            },
            {
                .description = "Capture specimens (Scientist)",
                .requirements = EV_FLAG_REQUIRES_SCIENTIST,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your scientist captures live specimens for study, yielding valuable research data.",
                        .name = "Your scientist captures live specimens for study, yielding valuable research data.",
                        .chance = 80,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = 600,
                            },
                            {
                                .type = EV_EFFECT_XP,
                                .value = 20,
                            },
                        },
                    },
                    {
                        .description = "During capture, a spider escapes and bites a crew member fatally.",
                        .name = "During capture, a spider escapes and bites a crew member fatally.",
                        .chance = 20,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = 500,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 23,
        .title = "Silence Across the Void",
        .description = "Your communication systems dissolve into white noise and static. Every channel screams with interference from an unknown source. You're cut off from the wider galaxy-blind and unable to reach help or contact anyone out there. Just you and the void.",
        .option_count = 4,
        .options = {
            {
                .description = "Wait",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You hold position and wait patiently. The interference clears as mysteriously as it came.",
                        .name = "You hold position and wait patiently. The interference clears as mysteriously as it came.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "Silence stretches on, but eventually the interference subsides.",
                        .name = "Silence stretches on, but eventually the interference subsides.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 50,
                            },
                        },
                    },
                },
            },
            {
                .description = "Boost signal",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Power surges through your communications array. The distortion breaks and clarity returns.",
                        .name = "Power surges through your communications array. The distortion breaks and clarity returns.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "The boost succeeds but drains your power reserves faster than expected.",
                        .name = "The boost succeeds but drains your power reserves faster than expected.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Ignore",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You proceed without communications. It's risky, but you maintain course.",
                        .name = "You proceed without communications. It's risky, but you maintain course.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "Critical information passes you by. You miss important signals and opportunities.",
                        .name = "Critical information passes you by. You miss important signals and opportunities.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = -50,
                            },
                        },
                    },
                },
            },
            {
                .description = "Calibrate (Engineer)",
                .requirements = EV_FLAG_REQUIRES_ENGINEER,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your engineer's calibration work is flawless. Communications scream through clearly, revealing a hidden cache location.",
                        .name = "Your engineer's calibration work is flawless. Communications scream through clearly, revealing a hidden cache location.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = 4,
                            },
                        },
                    },
                    {
                        .description = "The calibration improves things marginally, but requires careful engineering attention.",
                        .name = "The calibration improves things marginally, but requires careful engineering attention.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 50,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 25,
        .title = "Relics of the First Builders",
        .description = "Planetary sensors reveal monumental structures beneath the world's crust. Architecture that predates any known species, with purpose and scale that makes crew catch their breath. Could this be the legendary home of the First Builders? A discovery that could rewrite history itself.",
        .option_count = 4,
        .options = {
            {
                .description = "Explore",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your team discovers an underground chamber filled with priceless First Builder artifacts and technology.",
                        .name = "Your team discovers an underground chamber filled with priceless First Builder artifacts and technology.",
                        .chance = 50,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = 600,
                            },
                        },
                    },
                    {
                        .description = "Exploration triggers ancient defense systems. Structures collapse around you.",
                        .name = "Exploration triggers ancient defense systems. Structures collapse around you.",
                        .chance = 50,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -3,
                            },
                        },
                    },
                },
            },
            {
                .description = "Ignore",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Stay safe.",
                        .name = "Stay safe.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 50,
                            },
                        },
                    },
                    {
                        .description = "Miss opportunity.",
                        .name = "Miss opportunity.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = -50,
                            },
                        },
                    },
                },
            },
            {
                .description = "Scan for lifeforms (Medic)",
                .requirements = EV_FLAG_REQUIRES_MEDIC,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your medic's scans reveal a stranded lifeform. They join your crew, grateful for rescue and offering knowledge in return.",
                        .name = "Your medic's scans reveal a stranded lifeform. They join your crew, grateful for rescue and offering knowledge in return.",
                        .chance = 80,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = 1,
                            },
                            {
                                .type = EV_EFFECT_UPGRADE,
                                .value = 0,
                            },
                        },
                    },
                    {
                        .description = "The analysis triggers dormant security protocols. Ancient weapons activate.",
                        .name = "The analysis triggers dormant security protocols. Ancient weapons activate.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Analyze ruins (Scientist)",
                .requirements = EV_FLAG_REQUIRES_SCIENTIST,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your scientist cracks the ancient code. A blueprint for advanced technology is revealed.",
                        .name = "Your scientist cracks the ancient code. A blueprint for advanced technology is revealed.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_UPGRADE,
                                .value = 0,
                            },
                        },
                    },
                    {
                        .description = "The analysis triggers dormant security protocols. Ancient weapons activate.",
                        .name = "The analysis triggers dormant security protocols. Ancient weapons activate.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 26,
        .title = "Too Much, Too Heavy",
        .description = "Red warnings blare across every console: CARGO BAY OVERSTRESSED. Structural beams groan under dangerous weight. Engineers exchange worried glances: 'Captain, if we take any more stress, the bay could rupture. We need to act now.'",
        .option_count = 4,
        .options = {
            {
                .description = "Dump excess cargo",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You jettison the least valuable goods. The bay restabilizes without incident.",
                        .name = "You jettison the least valuable goods. The bay restabilizes without incident.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -5,
                            },
                        },
                    },
                    {
                        .description = "You dump too much too fast. The loss is more severe than projected.",
                        .name = "You dump too much too fast. The loss is more severe than projected.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -10,
                            },
                        },
                    },
                },
            },
            {
                .description = "Reorganize load",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your crew redistributes weight with surgical precision. The cargo bay achieves stable equilibrium.",
                        .name = "Your crew redistributes weight with surgical precision. The cargo bay achieves stable equilibrium.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -3,
                            },
                        },
                    },
                    {
                        .description = "The physical reorganization is exhausting. Your crew burns energy on tedious rearrangement.",
                        .name = "The physical reorganization is exhausting. Your crew burns energy on tedious rearrangement.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Ignore warnings",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You push forward at full speed. The overloaded bay holds-barely.",
                        .name = "You push forward at full speed. The overloaded bay holds-barely.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "Something has to give. Structural supports fail spectacularly.",
                        .name = "Something has to give. Structural supports fail spectacularly.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Optimize load (Engineer)",
                .requirements = EV_FLAG_REQUIRES_ENGINEER,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your engineer uses advanced techniques to pack everything weightlessly. Maximum cargo, zero risk.",
                        .name = "Your engineer uses advanced techniques to pack everything weightlessly. Maximum cargo, zero risk.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 200,
                            },
                        },
                    },
                    {
                        .description = "Some optimization is achieved, but not the deep improvement you hoped for.",
                        .name = "Some optimization is achieved, but not the deep improvement you hoped for.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -3,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 27,
        .title = "An Unexpected Passenger",
        .description = "Your security team drags out a frightened figure from the cargo hold-someone's been hiding in your ship, probably for days. Where did they come from? What do they want? And most importantly... are they a threat to you and your crew?",
        .option_count = 4,
        .options = {
            {
                .description = "Interrogate them",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "The stowaway reveals valuable intelligence about smuggling routes and black market operations.",
                        .name = "The stowaway reveals valuable intelligence about smuggling routes and black market operations.",
                        .chance = 60,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = 20,
                            },
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 10,
                            },
                        },
                    },
                    {
                        .description = "They become defensive and hostile. Tensions spike as your crew puts them in the brig.",
                        .name = "They become defensive and hostile. Tensions spike as your crew puts them in the brig.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Eject them",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You flush them out the airlock without ceremony. The risk is gone.",
                        .name = "You flush them out the airlock without ceremony. The risk is gone.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = -10,
                            },
                        },
                    },
                    {
                        .description = "As they eject, you realize they might have been useful... or valuable to someone.",
                        .name = "As they eject, you realize they might have been useful... or valuable to someone.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = -5,
                            },
                        },
                    },
                },
            },
            {
                .description = "Let them stay",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You grant them sanctuary. Days later, they prove invaluable during a crisis.",
                        .name = "You grant them sanctuary. Days later, they prove invaluable during a crisis.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = 1,
                            },
                        },
                    },
                    {
                        .description = "They appear grateful, then vanish with crucial data and expensive supplies. Betrayal.",
                        .name = "They appear grateful, then vanish with crucial data and expensive supplies. Betrayal.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Assess threat (Security)",
                .requirements = EV_FLAG_REQUIRES_SOLDIER,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your security team reads the stowaway correctly. You make the best decision for your ship.",
                        .name = "Your security team reads the stowaway correctly. You make the best decision for your ship.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 10,
                            },
                        },
                    },
                    {
                        .description = "Your team misreads the situation. You take the wrong action, and it costs you.",
                        .name = "Your team misreads the situation. You take the wrong action, and it costs you.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 29,
        .title = "Bleeding Through Space",
        .description = "Your fuel indicator twitches downward with alarming speed. The engineers report catastrophic news: a rupture in the starboard fuel line. Your precious reserves are leaking into the void, leaving a trail of antimatter vapor that glows like a beacon to anyone hunting you.",
        .option_count = 4,
        .options = {
            {
                .description = "Seal leak",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your engineers rapidly patch the rupture. Fuel loss is minimal and controlled.",
                        .name = "Your engineers rapidly patch the rupture. Fuel loss is minimal and controlled.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The patch holds, but the repair process is more demanding than you'd like.",
                        .name = "The patch holds, but the repair process is more demanding than you'd like.",
                        .chance = 40,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Ignore temporarily",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You maintain course and pressure. The leak remains manageable for the short term.",
                        .name = "You maintain course and pressure. The leak remains manageable for the short term.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -2,
                            },
                        },
                    },
                    {
                        .description = "The leak becomes critical. A fuel ignition reaction erupts through the ship.",
                        .name = "The leak becomes critical. A fuel ignition reaction erupts through the ship.",
                        .chance = 40,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Dump fuel",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You vent all compromised fuel reserves. The hazard is completely neutralized.",
                        .name = "You vent all compromised fuel reserves. The hazard is completely neutralized.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -4,
                            },
                        },
                    },
                    {
                        .description = "You dump more fuel than necessary. The waste is catastrophic to your reserves.",
                        .name = "You dump more fuel than necessary. The waste is catastrophic to your reserves.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -5,
                            },
                        },
                    },
                },
            },
            {
                .description = "Patch system (Engineer)",
                .requirements = EV_FLAG_REQUIRES_ENGINEER,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your engineer applies a brilliant solution. Fuel loss is reduced to minimal amounts.",
                        .name = "Your engineer applies a brilliant solution. Fuel loss is reduced to minimal amounts.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = 0,
                            },
                        },
                    },
                    {
                        .description = "The patch works but isn't perfect. Some additional fuel still leaks away.",
                        .name = "The patch works but isn't perfect. Some additional fuel still leaks away.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 35,
        .title = "The Calling",
        .description = "Your comms array picks up an eerie pulse-a signal so elegant, so perfectly structured, that it can't possibly be natural. It repeats in a measured cadence, almost like morse code from a long-lost rocket-age beacon, but in no language your translators recognize. It pulls at your curiosity like nothing you've felt before.",
        .option_count = 4,
        .options = {
            {
                .description = "Investigate",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You trace the signal to an advanced alien probe containing unprecedented scientific data. Knowledge and reputation surge.",
                        .name = "You trace the signal to an advanced alien probe containing unprecedented scientific data. Knowledge and reputation surge.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "The signal was a lure. Armed satellites activate and begin targeting you.",
                        .name = "The signal was a lure. Armed satellites activate and begin targeting you.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Ignore",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You dismiss the signal and continue unimpeded. Safety prevails over curiosity.",
                        .name = "You dismiss the signal and continue unimpeded. Safety prevails over curiosity.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 10,
                            },
                        },
                    },
                    {
                        .description = "Later, you learn the signal contained investment opportunities worth millions. You missed them.",
                        .name = "Later, you learn the signal contained investment opportunities worth millions. You missed them.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = -100,
                            },
                        },
                    },
                },
            },
            {
                .description = "Record it",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You capture the signal perfectly. The signal contained great investment opportunities",
                        .name = "You capture the signal perfectly. The signal contained great investment opportunities",
                        .chance = 60,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 5,
                            },
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = 500,
                            },
                        },
                    },
                    {
                        .description = "The recording is corrupted with noise and interference. Analysis yields nothing useful.",
                        .name = "The recording is corrupted with noise and interference. Analysis yields nothing useful.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = -5,
                            },
                        },
                    },
                },
            },
            {
                .description = "Decode (Scientist)",
                .requirements = EV_FLAG_REQUIRES_SCIENTIST,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your scientist cracks the encrypted message. It reveals coordinates to an uncharted wealth cache.",
                        .name = "Your scientist cracks the encrypted message. It reveals coordinates to an uncharted wealth cache.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = 5,
                            },
                        },
                    },
                    {
                        .description = "As your scientist decodes it, the signal overloads your equipment. Systems blow violently.",
                        .name = "As your scientist decodes it, the signal overloads your equipment. Systems blow violently.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 36,
        .title = "Phantom Thieves",
        .description = "Inventory doesn't match. Valuable cargo has simply vanished from a locked hold with no sign of tampering. No alarms triggered. No security footage shows anything. It's as if someone walked through walls. Your crew is on edge. The black market, perhaps? Or something worse?",
        .option_count = 4,
        .options = {
            {
                .description = "Search ship",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your team finds the stolen goods hidden in a smuggler's compartment. Recovery complete.",
                        .name = "Your team finds the stolen goods hidden in a smuggler's compartment. Recovery complete.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = 5,
                            },
                        },
                    },
                    {
                        .description = "The cargo has disappeared without a trace. It's simply gone.",
                        .name = "The cargo has disappeared without a trace. It's simply gone.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -8,
                            },
                        },
                    },
                },
            },
            {
                .description = "Report it",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Authorities investigate and recover most of your cargo. You're reimbursed significantly.",
                        .name = "Authorities investigate and recover most of your cargo. You're reimbursed significantly.",
                        .chance = 60,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = 500,
                            },
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -5,
                            },
                        },
                    },
                    {
                        .description = "The bureaucratic investigation is exhausting and progress halts.",
                        .name = "The bureaucratic investigation is exhausting and progress halts.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = -50,
                            },
                        },
                    },
                },
            },
            {
                .description = "Ignore loss",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 1,
                .outcomes = {
                    {
                        .description = "You mark the loss and move on pragmatically. Nothing changes, nothing stops.",
                        .name = "You mark the loss and move on pragmatically. Nothing changes, nothing stops.",
                        .chance = 100,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -5,
                            },
                        },
                    },
                },
            },
            {
                .description = "Track thief (Security)",
                .requirements = EV_FLAG_REQUIRES_SOLDIER,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your security team traces the theft to its source. You recover everything plus a bonus.",
                        .name = "Your security team traces the theft to its source. You recover everything plus a bonus.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = 5,
                            },
                        },
                    },
                    {
                        .description = "The trail goes cold. You recover some items, but not all.",
                        .name = "The trail goes cold. You recover some items, but not all.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -3,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 37,
        .title = "Flames in the Void",
        .description = "Temperature gauges climb into the red. Your engines scream as thermal readings spike beyond safe limits. You smell scorched metal and ozone drifting through the ventilation. The chief engineer shouts over the alarms: 'Captain, we're about to blow! Do something!'",
        .option_count = 4,
        .options = {
            {
                .description = "Cool down",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your crew vents steam and reduces engine temperature. Normal operations continue unimpeded.",
                        .name = "Your crew vents steam and reduces engine temperature. Normal operations continue unimpeded.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The cooling process is difficult. The damage is mitigated but not prevented.",
                        .name = "The cooling process is difficult. The damage is mitigated but not prevented.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Push through",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your engines scream, but they hold. You arrive faster than expected despite the heat.",
                        .name = "Your engines scream, but they hold. You arrive faster than expected despite the heat.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                    {
                        .description = "You push too hard. Metal fails spectacularly under thermal stress.",
                        .name = "You push too hard. Metal fails spectacularly under thermal stress.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -3,
                            },
                        },
                    },
                },
            },
            {
                .description = "Reduce load",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Lighter cargo reduces engine strain significantly. Temperature normalizes.",
                        .name = "Lighter cargo reduces engine strain significantly. Temperature normalizes.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -3,
                            },
                        },
                    },
                    {
                        .description = "Even with reduced weight, the engines remain dangerously hot. You lose cargo for little gain.",
                        .name = "Even with reduced weight, the engines remain dangerously hot. You lose cargo for little gain.",
                        .chance = 40,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -8,
                            },
                        },
                    },
                },
            },
            {
                .description = "Optimize cooling (Engineer)",
                .requirements = EV_FLAG_REQUIRES_ENGINEER,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your engineer supercharges the cooling systems. Engine performance improves beyond baseline.",
                        .name = "Your engineer supercharges the cooling systems. Engine performance improves beyond baseline.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = 1,
                            },
                        },
                    },
                    {
                        .description = "Partial optimization is achieved. Cooling improves, but your overheating remains a concern.",
                        .name = "Partial optimization is achieved. Cooling improves, but your overheating remains a concern.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 38,
        .title = "Crushing Force",
        .description = "You enter a system that your databases said would be routine, but something is horrifically wrong. Gravity fields slam into your ship like a fist-stronger, far stronger than anything the local stellar bodies should produce. Your ship groans under the impossible strain. Something here defies known physics.",
        .option_count = 4,
        .options = {
            {
                .description = "Adjust thrusters",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your pilot makes delicate thruster adjustments. You descend smoothly despite the crushing gravity.",
                        .name = "Your pilot makes delicate thruster adjustments. You descend smoothly despite the crushing gravity.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The gravity is stronger than compensated for. Your ship shudders and takes damage.",
                        .name = "The gravity is stronger than compensated for. Your ship shudders and takes damage.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -3,
                            },
                        },
                    },
                },
            },
            {
                .description = "Avoid the area",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Prudent routing around the anomaly keeps you safe from harm.",
                        .name = "Prudent routing around the anomaly keeps you safe from harm.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The detour consumes fuel that you can't spare.",
                        .name = "The detour consumes fuel that you can't spare.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Push forward",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your crew braves the crushing forces and emerges victorious. The experience hardens them.",
                        .name = "Your crew braves the crushing forces and emerges victorious. The experience hardens them.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 200,
                            },
                        },
                    },
                    {
                        .description = "The gravity is more intense than anticipated. Your ship shudders and takes damage.",
                        .name = "The gravity is more intense than anticipated. Your ship shudders and takes damage.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Pilot adjust (Pilot)",
                .requirements = EV_FLAG_REQUIRES_PILOT,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your pilot's mastery of gravity fields is stunning. You gain extra speed and efficiency.",
                        .name = "Your pilot's mastery of gravity fields is stunning. You gain extra speed and efficiency.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = 1,
                            },
                        },
                    },
                    {
                        .description = "The adjustment is challenging. You maintain course but expend extra energy.",
                        .name = "The adjustment is challenging. You maintain course but expend extra energy.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 41,
        .title = "Diamonds in Darkness",
        .description = "Your contact transmits an encrypted message: 'I have a shipment that others will pay handsomely to intercept. Transport it to the outer colonies, no questions asked. The profits would set you up for years... but so would the risk if you're caught.'",
        .option_count = 4,
        .options = {
            {
                .description = "Accept",
                .requirements = EV_FLAG_REQUIRES_FUEL,
                .required_count = 2,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "The contract pays spectacularly. Your coffers overflow with credits... but you've made enemies.",
                        .name = "The contract pays spectacularly. Your coffers overflow with credits... but you've made enemies.",
                        .chance = 60,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = 90,
                            },
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -2,
                            },
                        },
                    },
                    {
                        .description = "The authorities catch you. Severe penalties and complications follow.",
                        .name = "The authorities catch you. Severe penalties and complications follow.",
                        .chance = 40,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = -100,
                            },
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Decline",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 1,
                .outcomes = {
                    {
                        .description = "You turn them down politely. No complications, no entanglement.",
                        .name = "You turn them down politely. No complications, no entanglement.",
                        .chance = 100,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 5,
                            },
                        },
                    },
                },
            },
            {
                .description = "Negotiate",
                .requirements = EV_FLAG_REQUIRES_FUEL,
                .required_count = 2,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your negotiation skills secure more favorable terms. The reward increases significantly.",
                        .name = "Your negotiation skills secure more favorable terms. The reward increases significantly.",
                        .chance = 60,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = 130,
                            },
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -2,
                            },
                        },
                    },
                    {
                        .description = "They refuse to improve their offer. You take it or leave it.",
                        .name = "They refuse to improve their offer. You take it or leave it.",
                        .chance = 40,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = 90,
                            },
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Secure route (Pilot)",
                .requirements = EV_FLAG_REQUIRES_PILOT,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your pilot charts a route that's both safe and profitable. The delivery succeeds cleanly.",
                        .name = "Your pilot charts a route that's both safe and profitable. The delivery succeeds cleanly.",
                        .chance = 80,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = 125,
                            },
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The route is safer, but enforcement patrols still get a sniff. Minor close calls occur.",
                        .name = "The route is safer, but enforcement patrols still get a sniff. Minor close calls occur.",
                        .chance = 20,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = -5,
                            },
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 42,
        .title = "Casualties of Transit",
        .description = "Jump trauma rattled your hull; one precious crate cracked open and its contents scattered in the hold. The crew scrambles: can you salvage value from chaos, or has this cargo run been ruined?",
        .option_count = 4,
        .options = {
            {
                .description = "Repair crate",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your engineers mend the crate and seal the shipment. Most cargo survives intact.",
                        .name = "Your engineers mend the crate and seal the shipment. Most cargo survives intact.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -2,
                            },
                        },
                    },
                    {
                        .description = "Repair materials prove more scarce than expected; you pay dearly in resources.",
                        .name = "Repair materials prove more scarce than expected; you pay dearly in resources.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -5,
                            },
                        },
                    },
                },
            },
            {
                .description = "Transfer contents",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Meticulous manual transfer recovers the bulk of goods, while the damaged few are quarantined.",
                        .name = "Meticulous manual transfer recovers the bulk of goods, while the damaged few are quarantined.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -2,
                            },
                        },
                    },
                    {
                        .description = "Chaos and clumsiness cause additional spillage. The effort saves little.",
                        .name = "Chaos and clumsiness cause additional spillage. The effort saves little.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -5,
                            },
                        },
                    },
                },
            },
            {
                .description = "Discard",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You throw the ruined crate away and avoid contamination, preserving the rest of the hold.",
                        .name = "You throw the ruined crate away and avoid contamination, preserving the rest of the hold.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -3,
                            },
                        },
                    },
                    {
                        .description = "The loss is steep, but it removes the hazard and allows flight to continue.",
                        .name = "The loss is steep, but it removes the hazard and allows flight to continue.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -5,
                            },
                        },
                    },
                },
            },
            {
                .description = "Reinforce (Engineer)",
                .requirements = EV_FLAG_REQUIRES_ENGINEER,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your engineer reinforces the crate with improvised plating. It survives and delivers valuable cargo.",
                        .name = "Your engineer reinforces the crate with improvised plating. It survives and delivers valuable cargo.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The reinforcement takes longer than expected and slows your departure.",
                        .name = "The reinforcement takes longer than expected and slows your departure.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -3,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 43,
        .title = "Illegal Inspection",
        .description = "Shadowy enforcement agents materialize at your airlock, clipboard in hand and blaster at their sides. They demand to board without jurisdiction-this is a dangerous grey market encounter.",
        .option_count = 4,
        .options = {
            {
                .description = "Comply",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You play the honest trader. The inspectors leave grudgingly convinced and your reputation improves.",
                        .name = "You play the honest trader. The inspectors leave grudgingly convinced and your reputation improves.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "Their over-zealous search finds contraband. They confiscate cargo and write a fine.",
                        .name = "Their over-zealous search finds contraband. They confiscate cargo and write a fine.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -5,
                            },
                        },
                    },
                },
            },
            {
                .description = "Flee",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You jump to lightspeed before they're ready. They lose you in the stars.",
                        .name = "You jump to lightspeed before they're ready. They lose you in the stars.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 5,
                            },
                        },
                    },
                    {
                        .description = "They pursue and manage a hit, leaving your hull scorched.",
                        .name = "They pursue and manage a hit, leaving your hull scorched.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Bribe",
                .requirements = EV_FLAG_REQUIRES_CREDITS,
                .required_count = 250,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "A crisp envelope changes hands. They leave with a wink.",
                        .name = "A crisp envelope changes hands. They leave with a wink.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -250,
                            },
                        },
                    },
                    {
                        .description = "They demand more and take cash anyway. You bleed funds.",
                        .name = "They demand more and take cash anyway. You bleed funds.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -500,
                            },
                        },
                    },
                },
            },
            {
                .description = "Negotiate (Security)",
                .requirements = EV_FLAG_REQUIRES_SOLDIER,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your security officer's negotiation skills impress the inspectors. They leave without incident.",
                        .name = "Your security officer's negotiation skills impress the inspectors. They leave without incident.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "The negotiation fails. The inspectors find a reason to fine you.",
                        .name = "The negotiation fails. The inspectors find a reason to fine you.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = -50,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 44,
        .title = "Broken Navigation Beacon",
        .description = "The trusted local nav beacon has gone dark. Charts now show empty void where warnings used to be. You must choose between speed, risk, and caution.",
        .option_count = 4,
        .options = {
            {
                .description = "Proceed blindly",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Careful instincts guide you with surprising accuracy and you gain ground quickly.",
                        .name = "Careful instincts guide you with surprising accuracy and you gain ground quickly.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "Without the beacon, your pilot misjudges a turn and the hull grazes asteroid debris.",
                        .name = "Without the beacon, your pilot misjudges a turn and the hull grazes asteroid debris.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Wait for repair",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You wait. The beacon is repaired and the route is secure.",
                        .name = "You wait. The beacon is repaired and the route is secure.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "Your patience pays off, and you avoid danger by staying put.",
                        .name = "Your patience pays off, and you avoid danger by staying put.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Use backup",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Backup navigators keep you on course, though not as efficiently as the official beacon.",
                        .name = "Backup navigators keep you on course, though not as efficiently as the official beacon.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The fallback guidance is outdated, causing extra fuel consumption.",
                        .name = "The fallback guidance is outdated, causing extra fuel consumption.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Calibrate (Pilot)",
                .requirements = EV_FLAG_REQUIRES_PILOT,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your pilot throws the nav system into a fine-tuned state and achieves near-perfect tracking.",
                        .name = "Your pilot throws the nav system into a fine-tuned state and achieves near-perfect tracking.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "Even expert pilots can only do so much; you drift slightly and burn extra fuel.",
                        .name = "Even expert pilots can only do so much; you drift slightly and burn extra fuel.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 45,
        .title = "Mysterious Illness",
        .description = "One by one, crew members begin coughing blood and vomiting teal foam. The medical bay is full. No one knows whether this is contagion, radiation, or something alien. Time and treatment are both critical.",
        .option_count = 3,
        .options = {
            {
                .description = "Quarantine",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your quick action prevents spread, though one crew member still succumbs.",
                        .name = "Your quick action prevents spread, though one crew member still succumbs.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The sickness already reached too far. Lives are lost despite containment.",
                        .name = "The sickness already reached too far. Lives are lost despite containment.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Ignore",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You risk it and keep moving. No one else gets sick immediately, but tensions rise.",
                        .name = "You risk it and keep moving. No one else gets sick immediately, but tensions rise.",
                        .chance = 60,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = -5,
                            },
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "Unchecked, the illness explodes through the crew. Many fall ill.",
                        .name = "Unchecked, the illness explodes through the crew. Many fall ill.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -3,
                            },
                        },
                    },
                },
            },
            {
                .description = "Cure it (Medic)",
                .requirements = EV_FLAG_REQUIRES_MEDIC,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your medic identifies an antidote from ship stores. Recovery begins rapidly.",
                        .name = "Your medic identifies an antidote from ship stores. Recovery begins rapidly.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "The experimental treatment backfires; the crew suffers uncertainty.",
                        .name = "The experimental treatment backfires; the crew suffers uncertainty.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 47,
        .title = "Hostile Vessel on Approach",
        .description = "Your radar catches a sleek, black destroyer closing fast. Its signal pings in a known pirate frequency. The HUD flashes red: prepare for possible combat or parley.",
        .option_count = 4,
        .options = {
            {
                .description = "Negotiate",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You offer tribute and they accept. Peace is bought at a cost.",
                        .name = "You offer tribute and they accept. Peace is bought at a cost.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -100,
                            },
                        },
                    },
                    {
                        .description = "The negotiations stall, and the enemy makes their move.",
                        .name = "The negotiations stall, and the enemy makes their move.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Evade",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You launch a daring escape through tight asteroid lanes and they fail to follow.",
                        .name = "You launch a daring escape through tight asteroid lanes and they fail to follow.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "They anticipate your escape vector and pin you down.",
                        .name = "They anticipate your escape vector and pin you down.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -3,
                            },
                        },
                    },
                },
            },
            {
                .description = "Fight",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You fight with naval precision and drive them off, though your hull is scarred.",
                        .name = "You fight with naval precision and drive them off, though your hull is scarred.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                    {
                        .description = "Their firepower is overwhelming. Your ship is wounded in the exchange.",
                        .name = "Their firepower is overwhelming. Your ship is wounded in the exchange.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -3,
                            },
                        },
                    },
                },
            },
            {
                .description = "Call for backup (Security)",
                .requirements = EV_FLAG_REQUIRES_SOLDIER,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Support arrives as if on cue. The hostile ship is routed.",
                        .name = "Support arrives as if on cue. The hostile ship is routed.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 10,
                            },
                        },
                    },
                    {
                        .description = "Backup arrives late. You fend them off but not without minor damage.",
                        .name = "Backup arrives late. You fend them off but not without minor damage.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 50,
        .title = "Ancient Defense System",
        .description = "As you explore the ancient ruins, dormant turrets roar to life and laser grids snap into place. Your ship is suddenly the target of a long-dead civilizations automated guardian.",
        .option_count = 4,
        .options = {
            {
                .description = "Evade",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You weave through beams and evade the worst of it, minimizing damage.",
                        .name = "You weave through beams and evade the worst of it, minimizing damage.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                    {
                        .description = "A stray pulse hits your forward plating and your hull is peppered with holes.",
                        .name = "A stray pulse hits your forward plating and your hull is peppered with holes.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -3,
                            },
                        },
                    },
                },
            },
            {
                .description = "Defend",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your ship hold and your crew returns fire, disabling several turrets.",
                        .name = "Your ship hold and your crew returns fire, disabling several turrets.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                    {
                        .description = "The laser barrage is relentless; your hull takes heavy punishment.",
                        .name = "The laser barrage is relentless; your hull takes heavy punishment.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -3,
                            },
                        },
                    },
                },
            },
            {
                .description = "Retreat",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You disengage and pull out of range, burning some extra fuel to flee.",
                        .name = "You disengage and pull out of range, burning some extra fuel to flee.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "Retreat is messy; the automated systems fire as you go, scraping your hull.",
                        .name = "Retreat is messy; the automated systems fire as you go, scraping your hull.",
                        .chance = 40,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Hack system (Scientist)",
                .requirements = EV_FLAG_REQUIRES_SCIENTIST,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your scientist overrides the ruins' AI and turns the defenses against false targets.",
                        .name = "Your scientist overrides the ruins' AI and turns the defenses against false targets.",
                        .chance = 80,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = 1,
                            },
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 10,
                            },
                        },
                    },
                    {
                        .description = "The system resists the hack and retaliates with focused beams.",
                        .name = "The system resists the hack and retaliates with focused beams.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 51,
        .title = "Hull Breach",
        .description = "A whistling rupture tears open your hull and an aft corridor disappears into black vacuum. Alarms shriek. Your crew's lives hang by a thread.",
        .option_count = 4,
        .options = {
            {
                .description = "Seal bulkheads immediately",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your rapid response locks down the breach before more air is lost. The crew nods in relief.",
                        .name = "Your rapid response locks down the breach before more air is lost. The crew nods in relief.",
                        .chance = 70,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                    {
                        .description = "The breach spreads faster than your seals can deploy, costing a life.",
                        .name = "The breach spreads faster than your seals can deploy, costing a life.",
                        .chance = 30,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Rescue exposed crew",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "A daring tether rescue pulls everyone back in. You've earned a hero's reputation.",
                        .name = "A daring tether rescue pulls everyone back in. You've earned a hero's reputation.",
                        .chance = 50,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 10,
                            },
                        },
                    },
                    {
                        .description = "The vacuum claims two as the rescue attempt goes badly.",
                        .name = "The vacuum claims two as the rescue attempt goes badly.",
                        .chance = 50,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Ignore and stabilize ship",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You sacrifice the damaged section and stabilize the vessel. Minimal further harm.",
                        .name = "You sacrifice the damaged section and stabilize the vessel. Minimal further harm.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                    {
                        .description = "Inaction leads to additional casualties. The crew is shaken.",
                        .name = "Inaction leads to additional casualties. The crew is shaken.",
                        .chance = 40,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Emergency patch (Engineer)",
                .requirements = EV_FLAG_REQUIRES_ENGINEER,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your engineer's clamp works perfectly; the breach is sealed instantly.",
                        .name = "Your engineer's clamp works perfectly; the breach is sealed instantly.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                    {
                        .description = "The patch holds but the rush causes an injury. One crew member is lost.",
                        .name = "The patch holds but the rush causes an injury. One crew member is lost.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 52,
        .title = "Radiation Surge",
        .description = "A sudden wave of star-borne gamma radiation engulfs the cargo bay during inspection. Geiger counters flash madly, and crew in under-pressure suits stumble. This is a race between shields and fallout.",
        .option_count = 4,
        .options = {
            {
                .description = "Evacuate immediately",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You shepherd everyone out. One crew member is exposed before the hatch slams.",
                        .name = "You shepherd everyone out. One crew member is exposed before the hatch slams.",
                        .chance = 70,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The evacuation is too slow; the blast affects one person fatally.",
                        .name = "The evacuation is too slow; the blast affects one person fatally.",
                        .chance = 30,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Shield cargo",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Cargo holds, and the loss is limited to a brief dose to ground crew.",
                        .name = "Cargo holds, and the loss is limited to a brief dose to ground crew.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "The shielding is inadequate. The crew suffers exposure despite your best effort.",
                        .name = "The shielding is inadequate. The crew suffers exposure despite your best effort.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Push through",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You persevere through the surge and finish locking down the cargo before more interference hits.",
                        .name = "You persevere through the surge and finish locking down the cargo before more interference hits.",
                        .chance = 50,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The radiation burns through armor. A crew member is lost.",
                        .name = "The radiation burns through armor. A crew member is lost.",
                        .chance = 50,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Calibrate shields (Scientist)",
                .requirements = EV_FLAG_REQUIRES_SCIENTIST,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your scientist perfects the pattern and the surge is routed safely.",
                        .name = "Your scientist perfects the pattern and the surge is routed safely.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The calibration is imperfect; the crew takes a minor hit.",
                        .name = "The calibration is imperfect; the crew takes a minor hit.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 53,
        .title = "Hostile Boarding",
        .description = "Pirate grapples hook into your hull and boarding ladders unfurl. Raiders blow past defenses into your corridor. Battle for the ship begins in the dark.",
        .option_count = 4,
        .options = {
            {
                .description = "Fight them off",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your crew forms a fighting line and drives the invaders back with fire and steel.",
                        .name = "Your crew forms a fighting line and drives the invaders back with fire and steel.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "The door is breached and the cost is blood. You lose a team member.",
                        .name = "The door is breached and the cost is blood. You lose a team member.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Flee",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You jump to hyperspace with jump drives screaming. The pirates can't follow.",
                        .name = "You jump to hyperspace with jump drives screaming. The pirates can't follow.",
                        .chance = 70,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "In the rush to escape, a rear guard is left behind and lost.",
                        .name = "In the rush to escape, a rear guard is left behind and lost.",
                        .chance = 30,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Surrender cargo",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You trade goods for your crew's heads. The pirates haul away and vanish.",
                        .name = "You trade goods for your crew's heads. The pirates haul away and vanish.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -10,
                            },
                        },
                    },
                    {
                        .description = "They greedily take hostages anyway for a later ransom.",
                        .name = "They greedily take hostages anyway for a later ransom.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Defensive tactics (Security)",
                .requirements = EV_FLAG_REQUIRES_SOLDIER,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your security teams execute perfect countermeasures. Boarding is repelled cleanly.",
                        .name = "Your security teams execute perfect countermeasures. Boarding is repelled cleanly.",
                        .chance = 85,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 150,
                            },
                        },
                    },
                    {
                        .description = "A panic moment causes one casualty, but the ship lives.",
                        .name = "A panic moment causes one casualty, but the ship lives.",
                        .chance = 15,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 54,
        .title = "Toxic Atmosphere",
        .description = "A corrosive vapor leaks from an unknown source; alarms strobe and the air tastes of metal and bleach. You must contain it before it overwhelms your lungs.",
        .option_count = 3,
        .options = {
            {
                .description = "Use suits",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your crew dons emergency anti-toxin suits and works carefully. The risk is averted.",
                        .name = "Your crew dons emergency anti-toxin suits and works carefully. The risk is averted.",
                        .chance = 70,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "One suit malfunctions. A crew member suffocates despite your best effort.",
                        .name = "One suit malfunctions. A crew member suffocates despite your best effort.",
                        .chance = 30,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Work quickly and fix the leak",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Rapid action seals the source. You breathe easier and gain a hard-fought victory.",
                        .name = "Rapid action seals the source. You breathe easier and gain a hard-fought victory.",
                        .chance = 50,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "The leak is stubborn. A crew member succumbs to exposure in the rush.",
                        .name = "The leak is stubborn. A crew member succumbs to exposure in the rush.",
                        .chance = 50,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Filter toxins (Engineer)",
                .requirements = EV_FLAG_REQUIRES_ENGINEER,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your engineer tunes the filters to maximum efficiency and clears the air quickly.",
                        .name = "Your engineer tunes the filters to maximum efficiency and clears the air quickly.",
                        .chance = 75,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "Filters cover most of it, but one toxic patch slips through.",
                        .name = "Filters cover most of it, but one toxic patch slips through.",
                        .chance = 25,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 55,
        .title = "Mutiny at Starboard",
        .description = "Whispers spread through the crew. Some demand action, others bristle at leadership. If this standoff explodes, your voyage may end in blood.",
        .option_count = 4,
        .options = {
            {
                .description = "Negotiate",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You broker a compromise. The crew calms and respect for your command is restored.",
                        .name = "You broker a compromise. The crew calms and respect for your command is restored.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "Negotiations collapse into fistfights, and one crew member is lost.",
                        .name = "Negotiations collapse into fistfights, and one crew member is lost.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Suppress forcefully",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your loyal security detains the mutineers and order returns.",
                        .name = "Your loyal security detains the mutineers and order returns.",
                        .chance = 70,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 50,
                            },
                        },
                    },
                    {
                        .description = "The suppression turns brutal; rebels fall and morale fractures.",
                        .name = "The suppression turns brutal; rebels fall and morale fractures.",
                        .chance = 30,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Give concessions",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You make difficult compromises. The crew cheers, but you spend resources to keep them content.",
                        .name = "You make difficult compromises. The crew cheers, but you spend resources to keep them content.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -300,
                            },
                        },
                    },
                    {
                        .description = "Your concessions are seen as weakness; mutiny plots deepen.",
                        .name = "Your concessions are seen as weakness; mutiny plots deepen.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Command authority (Security)",
                .requirements = EV_FLAG_REQUIRES_SOLDIER,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "A show of force and a firm command voice ends the uprising quickly.",
                        .name = "A show of force and a firm command voice ends the uprising quickly.",
                        .chance = 85,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 150,
                            },
                        },
                    },
                    {
                        .description = "The tactic saves the ship, but one life is lost in the chaos.",
                        .name = "The tactic saves the ship, but one life is lost in the chaos.",
                        .chance = 15,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 56,
        .title = "Venom of the Void",
        .description = "A crewman collapses with wet, black tendrils moving under their skin. The ship's biohazard sensors scream. This parasite is not from any known biology.",
        .option_count = 4,
        .options = {
            {
                .description = "Quarantine them",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "The split-level quarantine holds. The parasite is locked away.",
                        .name = "The split-level quarantine holds. The parasite is locked away.",
                        .chance = 70,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 50,
                            },
                        },
                    },
                    {
                        .description = "It breaks containment and infects another crew member.",
                        .name = "It breaks containment and infects another crew member.",
                        .chance = 30,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Eject infected",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your auto-lock releases the infected pod into open space. You save the ship at a harsh cost.",
                        .name = "Your auto-lock releases the infected pod into open space. You save the ship at a harsh cost.",
                        .chance = 90,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "It bursts before ejection and spores scatter. The parasite jumps to two more.",
                        .name = "It bursts before ejection and spores scatter. The parasite jumps to two more.",
                        .chance = 10,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Do nothing",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Strangely, the parasite goes dormant. You continue with uneasy calm.",
                        .name = "Strangely, the parasite goes dormant. You continue with uneasy calm.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = -5,
                            },
                        },
                    },
                    {
                        .description = "It spreads like wildfire and cripples your crew.",
                        .name = "It spreads like wildfire and cripples your crew.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Treat infection (Medic)",
                .requirements = EV_FLAG_REQUIRES_MEDIC,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "A medic develops an antiparasitic regimen. The crew's faith in your leadership is renewed.",
                        .name = "A medic develops an antiparasitic regimen. The crew's faith in your leadership is renewed.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "The treatment is not enough. One life is still lost.",
                        .name = "The treatment is not enough. One life is still lost.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 58,
        .title = "Daring Rocket Run",
        .description = "An opening appears in asteroid traffic, but it's narrow and uncharted. You must choose between careful pacing and a dangerous sprint. Heroics or catastrophe await.",
        .option_count = 3,
        .options = {
            {
                .description = "Take it slow",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You thread the route cautiously and arrive intact.",
                        .name = "You thread the route cautiously and arrive intact.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "Despite caution, a loose fragment clips your hull.",
                        .name = "Despite caution, a loose fragment clips your hull.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Push limits",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your risk pays off and you fly through with blazing speed.",
                        .name = "Your risk pays off and you fly through with blazing speed.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = 1,
                            },
                        },
                    },
                    {
                        .description = "The maneuver is too aggressive. You smash a wing and suffer heavy damage.",
                        .name = "The maneuver is too aggressive. You smash a wing and suffer heavy damage.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -3,
                            },
                        },
                    },
                },
            },
            {
                .description = "Precision navigation (Pilot)",
                .requirements = EV_FLAG_REQUIRES_PILOT,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your pilot's calculations are perfect. The move becomes a legend in your logbook.",
                        .name = "Your pilot's calculations are perfect. The move becomes a legend in your logbook.",
                        .chance = 85,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 100,
                            },
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = 1,
                            },
                        },
                    },
                    {
                        .description = "Even precision cannot account for a rogue rock. Minor hull damage occurs.",
                        .name = "Even precision cannot account for a rogue rock. Minor hull damage occurs.",
                        .chance = 15,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 59,
        .title = "Cargo Bomb in the Hold",
        .description = "A dull thump turns into a heartbeat. Your cargo being starts to hiss and glow. One wrong move and your hold becomes a warhead.",
        .option_count = 4,
        .options = {
            {
                .description = "Dump cargo",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You eject the volatile containers safely. The blast occurs far behind you.",
                        .name = "You eject the volatile containers safely. The blast occurs far behind you.",
                        .chance = 90,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -10,
                            },
                        },
                    },
                    {
                        .description = "An explosion rips through the ejected mass before it clears. Shockwaves hit your hull.",
                        .name = "An explosion rips through the ejected mass before it clears. Shockwaves hit your hull.",
                        .chance = 10,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -5,
                            },
                        },
                    },
                },
            },
            {
                .description = "Stabilize it",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your crew applies cooling and dampeners; the materials settle.",
                        .name = "Your crew applies cooling and dampeners; the materials settle.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -2,
                            },
                        },
                    },
                    {
                        .description = "The booms begin despite your skillful work. The ship shudders through the blast.",
                        .name = "The booms begin despite your skillful work. The ship shudders through the blast.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Ignore",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Miraculously, the cargo is inert and nothing happens.",
                        .name = "Miraculously, the cargo is inert and nothing happens.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The hold explodes, and the ship nearly tears apart.",
                        .name = "The hold explodes, and the ship nearly tears apart.",
                        .chance = 60,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -3,
                            },
                        },
                    },
                },
            },
            {
                .description = "Disarm safely (Engineer)",
                .requirements = EV_FLAG_REQUIRES_ENGINEER,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your engineer's deft hands neutralize the charges with zero incidents.",
                        .name = "Your engineer's deft hands neutralize the charges with zero incidents.",
                        .chance = 85,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "A stray spark triggers a partial blast. One crew member is hurt.",
                        .name = "A stray spark triggers a partial blast. One crew member is hurt.",
                        .chance = 15,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 60,
        .title = "Lost in Storm",
        .description = "Lightning spikes and gravitational chaos toss your hull like a toy. Your navigation instruments spin and starfields warp. You must decide whether to fight it or wait.",
        .option_count = 3,
        .options = {
            {
                .description = "Wait it out",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "The storm eventually calms and your ship is intact.",
                        .name = "The storm eventually calms and your ship is intact.",
                        .chance = 70,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "The storm rips your sensors apart and you're lost in the void.",
                        .name = "The storm rips your sensors apart and you're lost in the void.",
                        .chance = 30,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Push forward",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You barrel through with grit. Progress is made despite the turmoil.",
                        .name = "You barrel through with grit. Progress is made despite the turmoil.",
                        .chance = 50,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "The storm punches through your wing. You take hull damage.",
                        .name = "The storm punches through your wing. You take hull damage.",
                        .chance = 50,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Navigate storm (Pilot)",
                .requirements = EV_FLAG_REQUIRES_PILOT,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your pilot reads the storm like a poem and finds a calm corridor.",
                        .name = "Your pilot reads the storm like a poem and finds a calm corridor.",
                        .chance = 85,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "A sudden vortex catches you; minor hull stress results.",
                        .name = "A sudden vortex catches you; minor hull stress results.",
                        .chance = 15,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 61,
        .title = "Sabotaged Systems",
        .description = "Alarms blare as critical subsystems fail without warning. Someone with inside access is rerouting your power. You've been sabotaged in the middle of flight.",
        .option_count = 4,
        .options = {
            {
                .description = "Repair systems",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You identify the damaged modules and patch them quickly, restoring function.",
                        .name = "You identify the damaged modules and patch them quickly, restoring function.",
                        .chance = 70,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "Your repairs come moments too late; a cascade damages the hull.",
                        .name = "Your repairs come moments too late; a cascade damages the hull.",
                        .chance = 30,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Shut everything down",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "A cold reboot halts the sabotage and lets you diagnose the issue.",
                        .name = "A cold reboot halts the sabotage and lets you diagnose the issue.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "Partial systems collapse as you reboot, causing one critical failure.",
                        .name = "Partial systems collapse as you reboot, causing one critical failure.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Ignore sabotage",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You carry on and it somehow holds. The risk is enormous, but you survive.",
                        .name = "You carry on and it somehow holds. The risk is enormous, but you survive.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "The sabotage spreads and a system fails catastrophically.",
                        .name = "The sabotage spreads and a system fails catastrophically.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Trace saboteur (Security)",
                .requirements = EV_FLAG_REQUIRES_SOLDIER,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your security team catches the culprit and prevents further damage.",
                        .name = "Your security team catches the culprit and prevents further damage.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "The saboteur fights back. One systems engineer is lost in the crossfire.",
                        .name = "The saboteur fights back. One systems engineer is lost in the crossfire.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 63,
        .title = "Derelict Trap",
        .description = "You spot a drifting warship, marred by age and battle scars-too tempting for a salvage run. This could be winnings or a baited killzone. Your sensors warn of decay and hidden mines.",
        .option_count = 4,
        .options = {
            {
                .description = "Investigate",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You slip aboard and strip out the cargo, finding rare goods before attackers appear.",
                        .name = "You slip aboard and strip out the cargo, finding rare goods before attackers appear.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = 1000,
                            },
                        },
                    },
                    {
                        .description = "Pirates spring from the shadows and you're ambushed mid-salvage.",
                        .name = "Pirates spring from the shadows and you're ambushed mid-salvage.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Avoid it",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You choose caution. The ship drifts away, and you avoid possible death.",
                        .name = "You choose caution. The ship drifts away, and you avoid possible death.",
                        .chance = 90,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 10,
                            },
                        },
                    },
                    {
                        .description = "It was an ambush after all. You miss the trap but lose out on potential salvage.",
                        .name = "It was an ambush after all. You miss the trap but lose out on potential salvage.",
                        .chance = 10,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = -10,
                            },
                        },
                    },
                },
            },
            {
                .description = "Salvage quickly",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You rush in, grab the best salvage, and jump out before the trap snaps.",
                        .name = "You rush in, grab the best salvage, and jump out before the trap snaps.",
                        .chance = 50,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = 1500,
                            },
                        },
                    },
                    {
                        .description = "The trap triggers within seconds; one crew member is lost in the explosion.",
                        .name = "The trap triggers within seconds; one crew member is lost in the explosion.",
                        .chance = 50,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Scan first (Scientist)",
                .requirements = EV_FLAG_REQUIRES_SCIENTIST,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your scan reveals the hidden triggers and you deactivate them, securing high-value loot.",
                        .name = "Your scan reveals the hidden triggers and you deactivate them, securing high-value loot.",
                        .chance = 85,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = 800,
                            },
                        },
                    },
                    {
                        .description = "You miss one trigger. A burst injures the boarding team.",
                        .name = "You miss one trigger. A burst injures the boarding team.",
                        .chance = 15,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 64,
        .title = "Escape Pod Decision",
        .description = "One of your crew radiates a desperate distress call from an escape pod drifting in the void-separated during a hull breach. Their panicked voice crackles through comms: 'Captain, I'm out here... please, don't leave me.' Life support is failing fast.",
        .option_count = 4,
        .options = {
            {
                .description = "Rescue them",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your team executes a flawless rescue. They return safely, bearing gratitude and loyalty.",
                        .name = "Your team executes a flawless rescue. They return safely, bearing gratitude and loyalty.",
                        .chance = 70,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "The rescue attempt takes a tragic turn. One of your rescuers doesn't come back.",
                        .name = "The rescue attempt takes a tragic turn. One of your rescuers doesn't come back.",
                        .chance = 30,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Leave them",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You continue the mission, hardening your heart. Reputation suffers but the mission succeeds.",
                        .name = "You continue the mission, hardening your heart. Reputation suffers but the mission succeeds.",
                        .chance = 90,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = -100,
                            },
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The crew is devastated. Morale collapses as guilt consumes them. Your reputation becomes toxic.",
                        .name = "The crew is devastated. Morale collapses as guilt consumes them. Your reputation becomes toxic.",
                        .chance = 10,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = -200,
                            },
                        },
                    },
                },
            },
            {
                .description = "Send drone",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your drone works perfectly. The stranded crew member is retrieved without risk.",
                        .name = "Your drone works perfectly. The stranded crew member is retrieved without risk.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 50,
                            },
                        },
                    },
                    {
                        .description = "The drone malfunctions during retrieval. Your crew member is lost before you can respond.",
                        .name = "The drone malfunctions during retrieval. Your crew member is lost before you can respond.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Coordinate rescue (Pilot)",
                .requirements = EV_FLAG_REQUIRES_PILOT,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your pilot orchestrates a textbook rescue. Your stranded crew member returns safely, forever grateful.",
                        .name = "Your pilot orchestrates a textbook rescue. Your stranded crew member returns safely, forever grateful.",
                        .chance = 85,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 150,
                            },
                        },
                    },
                    {
                        .description = "A miscalculation in the rescue vector costs a life. Your crew member is lost anyway.",
                        .name = "A miscalculation in the rescue vector costs a life. Your crew member is lost anyway.",
                        .chance = 15,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 65,
        .title = "Experimental Tech Failure",
        .description = "Alarms shriek through engineering as the experimental flux module overloads. Containment fields flicker, casting eerie blue glows while sparks explode from consoles. The air reeks of burning circuits. This promised breakthrough has become a catastrophic threat.",
        .option_count = 4,
        .options = {
            {
                .description = "Shut it down",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You power down the module carefully. It winds down safely.",
                        .name = "You power down the module carefully. It winds down safely.",
                        .chance = 70,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "Powering it down causes a cascade reaction. An explosion rocks your engineering bay.",
                        .name = "Powering it down causes a cascade reaction. An explosion rocks your engineering bay.",
                        .chance = 30,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Keep using it",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "The module runs beautifully, delivering performance beyond specifications. The cargo is now more valuable.",
                        .name = "The module runs beautifully, delivering performance beyond specifications. The cargo is now more valuable.",
                        .chance = 50,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = 2,
                            },
                        },
                    },
                    {
                        .description = "Without warning, the system explodes catastrophically. You lose multiple crew members.",
                        .name = "Without warning, the system explodes catastrophically. You lose multiple crew members.",
                        .chance = 50,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Remove module",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your engineers carefully extract the module from the system. No damage occurs.",
                        .name = "Your engineers carefully extract the module from the system. No damage occurs.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "During removal, a coolant line ruptures. Dangerous fluids spray across the compartment.",
                        .name = "During removal, a coolant line ruptures. Dangerous fluids spray across the compartment.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Stabilize tech (Scientist)",
                .requirements = EV_FLAG_REQUIRES_SCIENTIST,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your scientist stabilizes the exotic reactions. The module now runs reliably and efficiently.",
                        .name = "Your scientist stabilizes the exotic reactions. The module now runs reliably and efficiently.",
                        .chance = 85,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = 1,
                            },
                        },
                    },
                    {
                        .description = "Stabilization efforts are only partially successful. Performance remains unpredictable.",
                        .name = "Stabilization efforts are only partially successful. Performance remains unpredictable.",
                        .chance = 15,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 66,
        .title = "Micrometeorite Shower",
        .description = "Your ship enters a debris field teeming with tiny, fast-moving fragments. Pings cascade across your hull like rain. Each impact alone is insignificant, but the collective bombardment could spell disaster if you don't act.",
        .option_count = 3,
        .options = {
            {
                .description = "Navigate through",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Careful piloting threads between the fragments. You emerge unscathed.",
                        .name = "Careful piloting threads between the fragments. You emerge unscathed.",
                        .chance = 50,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "A shower of impacts pounds your forward plating. The damage accumulates quickly.",
                        .name = "A shower of impacts pounds your forward plating. The damage accumulates quickly.",
                        .chance = 50,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -3,
                            },
                        },
                    },
                },
            },
            {
                .description = "Reverse course",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You backtrack out of the field before serious damage occurs.",
                        .name = "You backtrack out of the field before serious damage occurs.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "Turning through the field exposes your side. Significant impacts rake your hull.",
                        .name = "Turning through the field exposes your side. Significant impacts rake your hull.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Plot a new course",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your navigation team finds a safe vector through the debris. The ship is spared.",
                        .name = "Your navigation team finds a safe vector through the debris. The ship is spared.",
                        .chance = 70,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "The new course is miscalculated. You collide with a dense cluster of fragments.",
                        .name = "The new course is miscalculated. You collide with a dense cluster of fragments.",
                        .chance = 30,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -3,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 67,
        .title = "Structural Fatigue Alert",
        .description = "Engineering reports concerning news: repeated stress from jumps and maneuvers has weakened the ship's frame. Hairline fractures spider across critical support beams. The structure is compromised.",
        .option_count = 3,
        .options = {
            {
                .description = "Emergency repairs",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Quick welding and bracing stabilize the most critical fractures.",
                        .name = "Quick welding and bracing stabilize the most critical fractures.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The damage is more extensive than anticipated. Repairs are incomplete.",
                        .name = "The damage is more extensive than anticipated. Repairs are incomplete.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -3,
                            },
                        },
                    },
                },
            },
            {
                .description = "Reinforce hull",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Extra plating is welded to the vulnerable sections. The structure strengthens.",
                        .name = "Extra plating is welded to the vulnerable sections. The structure strengthens.",
                        .chance = 70,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                    {
                        .description = "The reinforcement process is extensive and still leaves weak points.",
                        .name = "The reinforcement process is extensive and still leaves weak points.",
                        .chance = 30,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Continue carefully",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You proceed with caution. No immediate failure occurs.",
                        .name = "You proceed with caution. No immediate failure occurs.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "A sudden maneuver causes catastrophic failure. Large sections of hull rupture.",
                        .name = "A sudden maneuver causes catastrophic failure. Large sections of hull rupture.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -4,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 68,
        .title = "Unstable Reactor Surge",
        .description = "The reactor core temperature spikes wildly, and warning alarms shriek. The magnetic containment field wavers dangerously. One more surge could breach the containment and unleash catastrophic energy.",
        .option_count = 3,
        .options = {
            {
                .description = "Emergency dump",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You dump excess coolant into the reactor. Temperature normalizes safely.",
                        .name = "You dump excess coolant into the reactor. Temperature normalizes safely.",
                        .chance = 65,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The dump creates a thermal shock. Reactor casings crack and leak radiation.",
                        .name = "The dump creates a thermal shock. Reactor casings crack and leak radiation.",
                        .chance = 35,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Override safeties",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Manual control allows you to bleed off excess heat gradually. The crisis passes.",
                        .name = "Manual control allows you to bleed off excess heat gradually. The crisis passes.",
                        .chance = 55,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "The override triggers a safety cascade. The reactor explodes outward.",
                        .name = "The override triggers a safety cascade. The reactor explodes outward.",
                        .chance = 45,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -4,
                            },
                        },
                    },
                },
            },
            {
                .description = "Eject reactor",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You cut the reactor loose. It tumbles into space, taking the danger with it.",
                        .name = "You cut the reactor loose. It tumbles into space, taking the danger with it.",
                        .chance = 70,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -3,
                            },
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "Reactor ejection triggers an electromagnetic pulse that damages your hull.",
                        .name = "Reactor ejection triggers an electromagnetic pulse that damages your hull.",
                        .chance = 30,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 69,
        .title = "Collision Course",
        .description = "Your navigation officer shouts a warning: a rogue asteroid, knocked free by a recent stellar explosion, is on a direct intercept vector. There's barely time to react.",
        .option_count = 3,
        .options = {
            {
                .description = "Emergency maneuver",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your pilot executes a barrel roll. The asteroid misses by meters.",
                        .name = "Your pilot executes a barrel roll. The asteroid misses by meters.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The maneuver isn't quite enough. The asteroid clips your starboard wing.",
                        .name = "The maneuver isn't quite enough. The asteroid clips your starboard wing.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Ram it",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You punch through the asteroid. The fragments scatter harmlessly.",
                        .name = "You punch through the asteroid. The fragments scatter harmlessly.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                    {
                        .description = "The asteroid is denser than expected. The collision shatters your bow.",
                        .name = "The asteroid is denser than expected. The collision shatters your bow.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -4,
                            },
                        },
                    },
                },
            },
            {
                .description = "Jump to hyperspace",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You jump away just in time. Safe passage achieved.",
                        .name = "You jump away just in time. Safe passage achieved.",
                        .chance = 75,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -2,
                            },
                        },
                    },
                    {
                        .description = "The jump initiates too late. You're clipped as hyperspace forms.",
                        .name = "The jump initiates too late. You're clipped as hyperspace forms.",
                        .chance = 25,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -3,
                            },
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 70,
        .title = "Pressure Vessel Leak",
        .description = "A hissing sound echoes through the corridors. Your ship's main pressurization system is leaking, and the slow depressurization is spreading through multiple compartments. If unchecked, the crew will suffocate.",
        .option_count = 3,
        .options = {
            {
                .description = "Seal compartments",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Strategic sealing contains the leak to a single section. The crew survives.",
                        .name = "Strategic sealing contains the leak to a single section. The crew survives.",
                        .chance = 65,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The leak spreads faster than your seals. Multiple sections are lost.",
                        .name = "The leak spreads faster than your seals. Multiple sections are lost.",
                        .chance = 35,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -3,
                            },
                        },
                    },
                },
            },
            {
                .description = "Locate and patch",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your team finds the rupture and patches it quickly. Crisis averted.",
                        .name = "Your team finds the rupture and patches it quickly. Crisis averted.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The patch holds temporarily, but another rupture appears elsewhere.",
                        .name = "The patch holds temporarily, but another rupture appears elsewhere.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Reinitialize systems",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "A system reset stabilizes the pressurization. Normalcy returns.",
                        .name = "A system reset stabilizes the pressurization. Normalcy returns.",
                        .chance = 55,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The reset backfires, causing explosive decompression in the engineering bay.",
                        .name = "The reset backfires, causing explosive decompression in the engineering bay.",
                        .chance = 45,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -3,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 71,
        .title = "Corrosive Spill",
        .description = "A containment vessel ruptures in the cargo hold, spilling highly corrosive material across the deck plates. The substance eats through metal with terrifying speed, spreading toward critical systems.",
        .option_count = 3,
        .options = {
            {
                .description = "Flood the hold",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Flooding neutralizes the corrosive with minimal damage to your ship.",
                        .name = "Flooding neutralizes the corrosive with minimal damage to your ship.",
                        .chance = 65,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -2,
                            },
                        },
                    },
                    {
                        .description = "The flooding reaction is violent. Secondary explosions ripple through the hull.",
                        .name = "The flooding reaction is violent. Secondary explosions ripple through the hull.",
                        .chance = 35,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -3,
                            },
                        },
                    },
                },
            },
            {
                .description = "Deploy foam sealant",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Specialized foam contains the corrosion quickly and safely.",
                        .name = "Specialized foam contains the corrosion quickly and safely.",
                        .chance = 70,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The foam hardens incorrectly and cracks, allowing more corrosion.",
                        .name = "The foam hardens incorrectly and cracks, allowing more corrosion.",
                        .chance = 30,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Vent the hold",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Explosive decompression ejects the corrosive into space harmlessly.",
                        .name = "Explosive decompression ejects the corrosive into space harmlessly.",
                        .chance = 60,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                            {
                                .type = EV_EFFECT_CARGO,
                                .value = -4,
                            },
                        },
                    },
                    {
                        .description = "The venting process damages adjacent compartments severely.",
                        .name = "The venting process damages adjacent compartments severely.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -4,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 72,
        .title = "Solar Flare Impact",
        .description = "A massive solar flare erupts from the nearby star without warning. Radiation and charged particles slam into your unshielded side with tremendous force. Your ship shakes like a leaf in a hurricane.",
        .option_count = 3,
        .options = {
            {
                .description = "Rotate for protection",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Quick rotation positions a stronger section of hull toward the flare. Damage is mitigated.",
                        .name = "Quick rotation positions a stronger section of hull toward the flare. Damage is mitigated.",
                        .chance = 65,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The rotation is too slow. Your weakened hull takes the brunt of the impact.",
                        .name = "The rotation is too slow. Your weakened hull takes the brunt of the impact.",
                        .chance = 35,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -3,
                            },
                        },
                    },
                },
            },
            {
                .description = "Push engines",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Full throttle carries you out of the flare's path quickly.",
                        .name = "Full throttle carries you out of the flare's path quickly.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The acceleration destabilizes your hull already weakened by radiation.",
                        .name = "The acceleration destabilizes your hull already weakened by radiation.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Evasive maneuvers (Pilot)",
                .requirements = EV_FLAG_REQUIRES_PILOT,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your pilot's skill allows you to dodge the worst of the flare's effects.",
                        .name = "Your pilot's skill allows you to dodge the worst of the flare's effects.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "Even expert piloting can't avoid all damage. The ship suffers a moderate hit.",
                        .name = "Even expert piloting can't avoid all damage. The ship suffers a moderate hit.",
                        .chance = 20,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 73,
        .title = "Impact from Space Debris",
        .description = "A chunk of ancient satellite tumbles silently through space on a collision course. Your sensors barely detect it before impact. There's no time for evasion.",
        .option_count = 3,
        .options = {
            {
                .description = "Brace for impact",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your bracing procedures minimize the damage. You take a glancing blow.",
                        .name = "Your bracing procedures minimize the damage. You take a glancing blow.",
                        .chance = 65,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                    {
                        .description = "The impact is more violent than anticipated. Structural damage is severe.",
                        .name = "The impact is more violent than anticipated. Structural damage is severe.",
                        .chance = 35,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -3,
                            },
                        },
                    },
                },
            },
            {
                .description = "Evasive maneuvers",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your quick reflexes allow you to dodge the worst of the debris, but a fragment still strikes your hull.",
                        .name = "Your quick reflexes allow you to dodge the worst of the debris, but a fragment still strikes your hull.",
                        .chance = 55,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "your evasive action is too late. The debris hits a critical section of your ship.",
                        .name = "your evasive action is too late. The debris hits a critical section of your ship.",
                        .chance = 45,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Quick reactions (Pilot)",
                .requirements = EV_FLAG_REQUIRES_PILOT,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your pilot's instincts save the ship from catastrophic damage.",
                        .name = "Your pilot's instincts save the ship from catastrophic damage.",
                        .chance = 80,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "Even expert piloting can't avoid all damage. The ship suffers a moderate hit.",
                        .name = "Even expert piloting can't avoid all damage. The ship suffers a moderate hit.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 74,
        .title = "Accelerated Hull Decay",
        .description = "A rare isotope in the local space somehow accelerates corrosion on your ship's exterior. Your scanners show surface degradation at an alarming rate. The hull is literally rotting around you.",
        .option_count = 3,
        .options = {
            {
                .description = "Apply protective coating",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Protective layers are applied rapidly, slowing the decay significantly.",
                        .name = "Protective layers are applied rapidly, slowing the decay significantly.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The coating application is incomplete. Decay continues beneath the surface.",
                        .name = "The coating application is incomplete. Decay continues beneath the surface.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
            {
                .description = "Leave the system",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You flee the corrupted space and the decay stops immediately.",
                        .name = "You flee the corrupted space and the decay stops immediately.",
                        .chance = 75,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "You depart, but not before significant structural damage accumulates.",
                        .name = "You depart, but not before significant structural damage accumulates.",
                        .chance = 25,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -3,
                            },
                        },
                    },
                },
            },
            {
                .description = "Reinforce hull (Engineer)",
                .requirements = EV_FLAG_REQUIRES_ENGINEER,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your engineer's expertise allows for rapid reinforcement, halting the decay.",
                        .name = "Your engineer's expertise allows for rapid reinforcement, halting the decay.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The reinforcement is only partially successful. Some decay continues.",
                        .name = "The reinforcement is only partially successful. Some decay continues.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 75,
        .title = "Tidal Forces Tear",
        .description = "Your ship enters a region where gravitational tidal forces are extraordinarily powerful. Your vessel is caught between two massive stellar objects, each pulling on different parts of your ship with devastating force.",
        .option_count = 3,
        .options = {
            {
                .description = "Reinforce internal structure",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Strategic bracing prevents rupture. You endure the forces.",
                        .name = "Strategic bracing prevents rupture. You endure the forces.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                    {
                        .description = "The reinforcements are insufficient. The ship is torn partially.",
                        .name = "The reinforcements are insufficient. The ship is torn partially.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -4,
                            },
                        },
                    },
                },
            },
            {
                .description = "Use full power to escape",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Maximum thrust breaks you free of the tidal forces.",
                        .name = "Maximum thrust breaks you free of the tidal forces.",
                        .chance = 65,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_FUEL,
                                .value = -1,
                            },
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "The escape attempt stresses the hull beyond tolerance.",
                        .name = "The escape attempt stresses the hull beyond tolerance.",
                        .chance = 35,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -3,
                            },
                        },
                    },
                },
            },
            {
                .description = "Navigate carefully (Pilot)",
                .requirements = EV_FLAG_REQUIRES_PILOT,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your pilot's skill allows you to thread the gravitational currents safely.",
                        .name = "Your pilot's skill allows you to thread the gravitational currents safely.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                    {
                        .description = "A miscalculation causes a violent jolt. The ship suffers significant damage.",
                        .name = "A miscalculation causes a violent jolt. The ship suffers significant damage.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -2,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 76,
        .title = "Luxury Goods Auction Disaster",
        .description = "You've secured a shipment of rare luxury goods for trade at the upcoming sector auction. But the merchant guild suddenly announces new tariffs on imported goods-50% tax on all non-essential items. Your profits evaporate instantly.",
        .option_count = 3,
        .options = {
            {
                .description = "Pay the tariff",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You grudgingly pay the new tariff and proceed with reduced margins.",
                        .name = "You grudgingly pay the new tariff and proceed with reduced margins.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -150,
                            },
                        },
                    },
                    {
                        .description = "The tariff is even higher than announced. You lose substantial revenue.",
                        .name = "The tariff is even higher than announced. You lose substantial revenue.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -300,
                            },
                        },
                    },
                },
            },
            {
                .description = "Sell before auction",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You quickly offload the goods to avoid tariffs, taking a small loss.",
                        .name = "You quickly offload the goods to avoid tariffs, taking a small loss.",
                        .chance = 70,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -100,
                            },
                        },
                    },
                    {
                        .description = "You sell in a panic. Buyers exploit your desperation for deep discounts.",
                        .name = "You sell in a panic. Buyers exploit your desperation for deep discounts.",
                        .chance = 30,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -200,
                            },
                        },
                    },
                },
            },
            {
                .description = "Smuggle past customs",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your contraband slips through without detection. Tariffs avoided completely.",
                        .name = "Your contraband slips through without detection. Tariffs avoided completely.",
                        .chance = 50,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = 200,
                            },
                        },
                    },
                    {
                        .description = "Authorities catch your smuggling operation. Fines and penalties are severe.",
                        .name = "Authorities catch your smuggling operation. Fines and penalties are severe.",
                        .chance = 50,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -400,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 77,
        .title = "Medical Emergency Supply Cost",
        .description = "One of your crew members suffers a severe allergic reaction to something in the ship's water system. The medical supplies needed to treat them properly are expensive, and the nearest medical facility charges exorbitant prices for emergency drugs.",
        .option_count = 4,
        .options = {
            {
                .description = "Buy the expensive drugs",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "The treatment works perfectly. Your crew member makes a full recovery.",
                        .name = "The treatment works perfectly. Your crew member makes a full recovery.",
                        .chance = 80,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -250,
                            },
                            {
                                .type = EV_EFFECT_CREW,
                                .value = 0,
                            },
                        },
                    },
                    {
                        .description = "You purchase the drugs, but overcharging is rampant.",
                        .name = "You purchase the drugs, but overcharging is rampant.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -400,
                            },
                        },
                    },
                },
            },
            {
                .description = "Use generic medicine",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "The generic alternative works well enough. Your crew member recovers with minor complications.",
                        .name = "The generic alternative works well enough. Your crew member recovers with minor complications.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -50,
                            },
                        },
                    },
                    {
                        .description = "The generic medicine is ineffective. You're forced to buy the expensive option anyway.",
                        .name = "The generic medicine is ineffective. You're forced to buy the expensive option anyway.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -300,
                            },
                        },
                    },
                },
            },
            {
                .description = "Attempt to treat without medicine",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your crew member's condition stabilizes with careful monitoring and supportive care.",
                        .name = "Your crew member's condition stabilizes with careful monitoring and supportive care.",
                        .chance = 50,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 100,
                            },
                        },
                    },
                    {
                        .description = "Without proper medication, the allergic reaction worsens. Your crew member suffers long-term health issues.",
                        .name = "Without proper medication, the allergic reaction worsens. Your crew member suffers long-term health issues.",
                        .chance = 50,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Call for medical assistance (Medic)",
                .requirements = EV_FLAG_REQUIRES_MEDIC,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your medic's expertise allows for a precise treatment plan. The crew member recovers fully.",
                        .name = "Your medic's expertise allows for a precise treatment plan. The crew member recovers fully.",
                        .chance = 85,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -100,
                            },
                        },
                    },
                    {
                        .description = "Even with your medic's help, the situation is dire. The crew member suffers complications.",
                        .name = "Even with your medic's help, the situation is dire. The crew member suffers complications.",
                        .chance = 15,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREW,
                                .value = -1,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 78,
        .title = "Bribery Opportunity",
        .description = "A dockmaster demands payment to prioritize your docking request. Without it, you'll wait days. The unofficial 'expediting fee' is steep, but time is money in this business.",
        .option_count = 3,
        .options = {
            {
                .description = "Pay the bribe",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You grease the palm and dock immediately. Time and resources are saved.",
                        .name = "You grease the palm and dock immediately. Time and resources are saved.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -200,
                            },
                        },
                    },
                    {
                        .description = "The dockmaster demands additional payment after already receiving the bribe.",
                        .name = "The dockmaster demands additional payment after already receiving the bribe.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -400,
                            },
                        },
                    },
                },
            },
            {
                .description = "Wait your turn",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You wait patiently. Docking eventually happens, and you save the credits.",
                        .name = "You wait patiently. Docking eventually happens, and you save the credits.",
                        .chance = 70,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 20,
                            },
                        },
                    },
                    {
                        .description = "The delay is more expensive than the bribe would have been. Penalties accrue.",
                        .name = "The delay is more expensive than the bribe would have been. Penalties accrue.",
                        .chance = 30,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -150,
                            },
                        },
                    },
                },
            },
            {
                .description = "Report corruption",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Authorities investigate and validate your report. No bribe needed.",
                        .name = "Authorities investigate and validate your report. No bribe needed.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 50,
                            },
                        },
                    },
                    {
                        .description = "Your report angers the dockmaster. He charges you extra fees out of spite.",
                        .name = "Your report angers the dockmaster. He charges you extra fees out of spite.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -300,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 79,
        .title = "Cargo Damage Compensation",
        .description = "One of your recent deliveries arrived damaged due to inadequate packaging during transport. The buyer is demanding full compensation for the loss, threatening legal action and public reputation damage.",
        .option_count = 2,
        .options = {
            {
                .description = "Settle generously",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You compensate them fairly. They retract complaints and leave positive feedback.",
                        .name = "You compensate them fairly. They retract complaints and leave positive feedback.",
                        .chance = 70,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -200,
                            },
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 50,
                            },
                        },
                    },
                    {
                        .description = "You compensate them, but they still leave negative reviews.",
                        .name = "You compensate them, but they still leave negative reviews.",
                        .chance = 30,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -200,
                            },
                        },
                    },
                },
            },
            {
                .description = "Negotiate",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your negotiation skills secure a partial settlement. Both sides compromise.",
                        .name = "Your negotiation skills secure a partial settlement. Both sides compromise.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -100,
                            },
                        },
                    },
                    {
                        .description = "They refuse to negotiate. You're forced to pay full compensation plus penalties.",
                        .name = "They refuse to negotiate. You're forced to pay full compensation plus penalties.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -350,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 80,
        .title = "Inflated Repair Bill",
        .description = "Your ship needed emergency repairs at a remote station. The repair crew submits a bill that seems astronomical-well above standard rates and with mysterious 'expert surcharge' fees.",
        .option_count = 3,
        .options = {
            {
                .description = "Pay in full",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You pay the exorbitant bill and leave without complaint. The ship is repaired properly.",
                        .name = "You pay the exorbitant bill and leave without complaint. The ship is repaired properly.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -500,
                            },
                        },
                    },
                    {
                        .description = "You pay the bill, but the repairs prove inadequate. More work is needed.",
                        .name = "You pay the bill, but the repairs prove inadequate. More work is needed.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -600,
                            },
                        },
                    },
                },
            },
            {
                .description = "Negotiate",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your objections sway them to reduce the surcharge significantly.",
                        .name = "Your objections sway them to reduce the surcharge significantly.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -300,
                            },
                        },
                    },
                    {
                        .description = "They refuse to budge. The bill remains astronomically high.",
                        .name = "They refuse to budge. The bill remains astronomically high.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -500,
                            },
                        },
                    },
                },
            },
            {
                .description = "Leave without paying",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You slip away before they can enforce collection. No payment made.",
                        .name = "You slip away before they can enforce collection. No payment made.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = -100,
                            },
                        },
                    },
                    {
                        .description = "They file a warrant. Legal fees and enforcement actions drain your accounts.",
                        .name = "They file a warrant. Legal fees and enforcement actions drain your accounts.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -600,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 81,
        .title = "Black Market Deal Gone Bad",
        .description = "An informant offers a lucrative black market deal-but the payment is demanded upfront and the goods aren't guaranteed. You send the credits, but the delivery never happens.",
        .option_count = 4,
        .options = {
            {
                .description = "Write it off",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 1,
                .outcomes = {
                    {
                        .description = "You accept the loss philosophically. It's a harsh lesson in street justice.",
                        .name = "You accept the loss philosophically. It's a harsh lesson in street justice.",
                        .chance = 100,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -400,
                            },
                        },
                    },
                },
            },
            {
                .description = "Seek retaliation",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You find the scammer and recover some money through forceful negotiation.",
                        .name = "You find the scammer and recover some money through forceful negotiation.",
                        .chance = 50,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -200,
                            },
                        },
                    },
                    {
                        .description = "Your retaliation attempt escalates. You lose more money paying off angry associates.",
                        .name = "Your retaliation attempt escalates. You lose more money paying off angry associates.",
                        .chance = 50,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -600,
                            },
                        },
                    },
                },
            },
            {
                .description = "Report to authorities",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Authorities investigate and recover part of your lost funds.",
                        .name = "Authorities investigate and recover part of your lost funds.",
                        .chance = 70,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -200,
                            },
                        },
                    },
                    {
                        .description = "The authorities are slow to act. The scammer disappears, and you lose everything.",
                        .name = "The authorities are slow to act. The scammer disappears, and you lose everything.",
                        .chance = 30,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -400,
                            },
                        },
                    },
                },
            },
            {
                .description = "Resort to violence (Security)",
                .requirements = EV_FLAG_REQUIRES_SOLDIER,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your security team intimidates the scammer into returning your funds.",
                        .name = "Your security team intimidates the scammer into returning your funds.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -100,
                            },
                        },
                    },
                    {
                        .description = "The violent approach backfires. You face legal consequences and lose more money.",
                        .name = "The violent approach backfires. You face legal consequences and lose more money.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -500,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 82,
        .title = "Insurance Fraud Investigation",
        .description = "After filing an insurance claim for cargo loss, investigators suspect fraud. They freeze your claim while they conduct a lengthy inquiry, and you're forced to pay for the entire loss out of pocket.",
        .option_count = 2,
        .options = {
            {
                .description = "Cooperate fully",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your cooperation satisfies them. The investigation clears you and the claim is paid.",
                        .name = "Your cooperation satisfies them. The investigation clears you and the claim is paid.",
                        .chance = 70,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -100,
                            },
                        },
                    },
                    {
                        .description = "Despite cooperation, they deny the claim citing insufficient evidence.",
                        .name = "Despite cooperation, they deny the claim citing insufficient evidence.",
                        .chance = 30,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -400,
                            },
                        },
                    },
                },
            },
            {
                .description = "Hire legal representation",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your lawyer fights the investigation and wins the claim back, though legal fees apply.",
                        .name = "Your lawyer fights the investigation and wins the claim back, though legal fees apply.",
                        .chance = 65,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -150,
                            },
                        },
                    },
                    {
                        .description = "Legal battle drags on longer than expected. Fees accumulate significantly.",
                        .name = "Legal battle drags on longer than expected. Fees accumulate significantly.",
                        .chance = 35,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -300,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 83,
        .title = "Port Authority Inspection Fine",
        .description = "A routine port authority inspection finds minor violations in your cargo manifests. The fine is steep, but the alternative is impounding your entire shipment.",
        .option_count = 2,
        .options = {
            {
                .description = "Pay the fine",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You pay the fine promptly. Your cargo is released without further issues.",
                        .name = "You pay the fine promptly. Your cargo is released without further issues.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -250,
                            },
                        },
                    },
                    {
                        .description = "You pay the fine, but additional violations are discovered. More fines follow.",
                        .name = "You pay the fine, but additional violations are discovered. More fines follow.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -450,
                            },
                        },
                    },
                },
            },
            {
                .description = "Contest it",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your appeal is successful. The fine is reduced significantly.",
                        .name = "Your appeal is successful. The fine is reduced significantly.",
                        .chance = 50,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -80,
                            },
                        },
                    },
                    {
                        .description = "The contest fails. You pay the full fine plus penalty surcharges.",
                        .name = "The contest fails. You pay the full fine plus penalty surcharges.",
                        .chance = 50,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -400,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 85,
        .title = "Crew Member Debt",
        .description = "One of your crew members confesses to accumulated gambling debts at the last port. Local creditors have tracked him to your ship and demand payment from the captain or they'll take matters into their own hands.",
        .option_count = 4,
        .options = {
            {
                .description = "Pay the debt",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You cover the debt to protect your crew and maintain peace aboard ship.",
                        .name = "You cover the debt to protect your crew and maintain peace aboard ship.",
                        .chance = 80,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -300,
                            },
                        },
                    },
                    {
                        .description = "You pay the debt, but the creditor demands future 'insurance' payments.",
                        .name = "You pay the debt, but the creditor demands future 'insurance' payments.",
                        .chance = 20,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -500,
                            },
                        },
                    },
                },
            },
            {
                .description = "Refuse and leave port",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "You deny responsibility and depart quickly. The crew member is troubled but safe.",
                        .name = "You deny responsibility and depart quickly. The crew member is troubled but safe.",
                        .chance = 70,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = -50,
                            },
                        },
                    },
                    {
                        .description = "Local authorities intercept you before departure. Fines and complications ensue.",
                        .name = "Local authorities intercept you before departure. Fines and complications ensue.",
                        .chance = 30,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -250,
                            },
                        },
                    },
                },
            },
            {
                .description = "Negotiate with creditors",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your negotiation skills convince the creditors to accept a smaller payment.",
                        .name = "Your negotiation skills convince the creditors to accept a smaller payment.",
                        .chance = 60,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -150,
                            },
                        },
                    },
                    {
                        .description = "Negotiations fail. The creditors escalate their demands, and you pay more than expected.",
                        .name = "Negotiations fail. The creditors escalate their demands, and you pay more than expected.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -400,
                            },
                        },
                    },
                },
            },
            {
                .description = "Handle discreetly (Security)",
                .requirements = EV_FLAG_REQUIRES_SOLDIER,
                .required_count = 1,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Your security team handles the matter quietly, resolving the debt without public knowledge.",
                        .name = "Your security team handles the matter quietly, resolving the debt without public knowledge.",
                        .chance = 85,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -200,
                            },
                        },
                    },
                    {
                        .description = "The discreet handling backfires. The crew member's reputation suffers, and the debt remains.",
                        .name = "The discreet handling backfires. The crew member's reputation suffers, and the debt remains.",
                        .chance = 15,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = -300,
                            },
                        },
                    },
                },
            },
        },
    },
    {
        .id = 101,
        .title = "Signal in the Static",
        .description = "A carrier wave bleeds through the comms, older than any registered beacon. It repeats a short pattern, then a string of coordinates that drift every few seconds. Your navigator frowns: 'That is not a distress call. That is an index.'",
        .option_count = 3,
        .options = {
            {
                .description = "Follow the signal",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 1,
                .outcomes = {
                    {
                        .description = "You lock the drifting coordinates. They point to something buried in the shipping lanes.",
                        .name = "You lock the drifting coordinates. They point to something buried in the shipping lanes.",
                        .chance = 100,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_FLAG_SET,
                                .value = 0,
                            },
                            {
                                .type = EV_EFFECT_CHAIN,
                                .value = 102,
                            },
                        },
                    },
                },
            },
            {
                .description = "Decode it first (Scientist)",
                .requirements = EV_FLAG_REQUIRES_SCIENTIST,
                .required_count = 1,
                .outcome_count = 1,
                .outcomes = {
                    {
                        .description = "Your scientist strips the drift and finds a maker's mark beneath it. The First Builders. The crew goes quiet.",
                        .name = "Your scientist strips the drift and finds a maker's mark beneath it. The First Builders. The crew goes quiet.",
                        .chance = 100,
                        .effect_count = 3,
                        .effects = {
                            {
                                .type = EV_EFFECT_FLAG_SET,
                                .value = 0,
                            },
                            {
                                .type = EV_EFFECT_XP,
                                .value = 40,
                            },
                            {
                                .type = EV_EFFECT_CHAIN,
                                .value = 102,
                            },
                        },
                    },
                },
            },
            {
                .description = "Log it and move on",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 1,
                .outcomes = {
                    {
                        .description = "You note the frequency in the log. The signal fades behind you.",
                        .name = "You note the frequency in the log. The signal fades behind you.",
                        .chance = 100,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_XP,
                                .value = 10,
                            },
                        },
                    },
                },
            },
        },
        .req_flags = 0x00000000u,
        .forbid_flags = 0x00000001u,
        .once = 1,
        .at = 0,
        .weight = 9,
    },
    {
        .id = 102,
        .title = "The Cartographer's Cache",
        .description = "The coordinates resolve to a lattice of plated spheres strung between two dead satellites. One sphere is open and empty. The others hum. A faint path of scorch marks shows where someone tried to force one and lost.",
        .option_count = 3,
        .options = {
            {
                .description = "Cut one open",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "The plating peels back cleanly. Inside: a star chart etched into alloy, and a hull section worth salvaging.",
                        .name = "The plating peels back cleanly. Inside: a star chart etched into alloy, and a hull section worth salvaging.",
                        .chance = 65,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_FLAG_SET,
                                .value = 1,
                            },
                            {
                                .type = EV_EFFECT_ALLOY,
                                .value = 1,
                            },
                        },
                    },
                    {
                        .description = "The sphere discharges. The flash tears a panel off your hull before the chart tumbles free.",
                        .name = "The sphere discharges. The flash tears a panel off your hull before the chart tumbles free.",
                        .chance = 35,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_FLAG_SET,
                                .value = 1,
                            },
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                        },
                    },
                },
            },
            {
                .description = "Scan the lattice (Scientist)",
                .requirements = EV_FLAG_REQUIRES_SCIENTIST,
                .required_count = 1,
                .outcome_count = 1,
                .outcomes = {
                    {
                        .description = "The scan finds the safe sphere. It opens on its own. The chart points to a wreck already on your map.",
                        .name = "The scan finds the safe sphere. It opens on its own. The chart points to a wreck already on your map.",
                        .chance = 100,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_FLAG_SET,
                                .value = 1,
                            },
                            {
                                .type = EV_EFFECT_XP,
                                .value = 40,
                            },
                        },
                    },
                },
            },
            {
                .description = "Trade the chart to a passing hauler",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 1,
                .outcomes = {
                    {
                        .description = "The hauler pays well and jumps before you can change your mind. Whatever the chart held is theirs now.",
                        .name = "The hauler pays well and jumps before you can change your mind. Whatever the chart held is theirs now.",
                        .chance = 100,
                        .effect_count = 2,
                        .effects = {
                            {
                                .type = EV_EFFECT_FLAG_SET,
                                .value = 1,
                            },
                            {
                                .type = EV_EFFECT_CREDITS,
                                .value = 400,
                            },
                        },
                    },
                },
            },
        },
        .req_flags = 0x00000001u,
        .forbid_flags = 0x00000002u,
        .once = 1,
        .at = 0,
        .weight = 0,
    },
    {
        .id = 103,
        .title = "The Builders' Vault",
        .description = "Past the wreck's core a sealed door carries the same maker's mark as the chart. Behind it the air is still and warm. Something in there has been running for a very long time.",
        .option_count = 4,
        .options = {
            {
                .description = "Breach the door (Soldier)",
                .requirements = EV_FLAG_REQUIRES_SOLDIER,
                .required_count = 1,
                .outcome_count = 1,
                .outcomes = {
                    {
                        .description = "The charge takes the door clean off. Inside, a relic rests in a cradle of light. Your soldier lifts it free.",
                        .name = "The charge takes the door clean off. Inside, a relic rests in a cradle of light. Your soldier lifts it free.",
                        .chance = 100,
                        .effect_count = 3,
                        .effects = {
                            {
                                .type = EV_EFFECT_FLAG_SET,
                                .value = 2,
                            },
                            {
                                .type = EV_EFFECT_RELIC,
                                .value = 1,
                            },
                            {
                                .type = EV_EFFECT_UPGRADE,
                                .value = 0,
                            },
                        },
                    },
                },
            },
            {
                .description = "Read the glyphs (Scientist)",
                .requirements = EV_FLAG_REQUIRES_SCIENTIST,
                .required_count = 1,
                .outcome_count = 1,
                .outcomes = {
                    {
                        .description = "The door answers the chart's pattern and slides open. The relic inside seems to recognise you.",
                        .name = "The door answers the chart's pattern and slides open. The relic inside seems to recognise you.",
                        .chance = 100,
                        .effect_count = 4,
                        .effects = {
                            {
                                .type = EV_EFFECT_FLAG_SET,
                                .value = 2,
                            },
                            {
                                .type = EV_EFFECT_RELIC,
                                .value = 1,
                            },
                            {
                                .type = EV_EFFECT_XP,
                                .value = 80,
                            },
                            {
                                .type = EV_EFFECT_UPGRADE,
                                .value = 0,
                            },
                        },
                    },
                },
            },
            {
                .description = "Force it",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 2,
                .outcomes = {
                    {
                        .description = "Brute work and bent tools. The door gives. The relic is yours, and so is a cracked strut.",
                        .name = "Brute work and bent tools. The door gives. The relic is yours, and so is a cracked strut.",
                        .chance = 60,
                        .effect_count = 4,
                        .effects = {
                            {
                                .type = EV_EFFECT_FLAG_SET,
                                .value = 2,
                            },
                            {
                                .type = EV_EFFECT_RELIC,
                                .value = 1,
                            },
                            {
                                .type = EV_EFFECT_HULL,
                                .value = -1,
                            },
                            {
                                .type = EV_EFFECT_UPGRADE,
                                .value = 0,
                            },
                        },
                    },
                    {
                        .description = "The door does not move. The warmth behind it fades, as if something decided you were not worth the trouble.",
                        .name = "The door does not move. The warmth behind it fades, as if something decided you were not worth the trouble.",
                        .chance = 40,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = -50,
                            },
                        },
                    },
                },
            },
            {
                .description = "Leave it sealed",
                .requirements = 0,
                .required_count = 0,
                .outcome_count = 1,
                .outcomes = {
                    {
                        .description = "Some doors should stay shut. The crew respects the call.",
                        .name = "Some doors should stay shut. The crew respects the call.",
                        .chance = 100,
                        .effect_count = 1,
                        .effects = {
                            {
                                .type = EV_EFFECT_REPUTATION,
                                .value = 60,
                            },
                        },
                    },
                },
            },
        },
        .req_flags = 0x00000002u,
        .forbid_flags = 0x00000004u,
        .once = 1,
        .at = 3,
        .weight = 0,
    },
};
const int random_event_count = 69;
