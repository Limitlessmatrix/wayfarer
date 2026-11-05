# Wayfarer (Orion Trail) host build: stock badge firmware source + host platform layer.
#   make            -> build/libwayfarer.so  (core for tools/wayfarer.py desktop frontend)
#   make cli        -> build/emu-cli         (headless timed script runner, PNG screenshots)
#   make events     -> regenerate upstream/firmware/generated_events.c from events.yml
#   make badge      -> UF2 for the real badge via pico-sdk (needs: source ~/badge/env.sh)
U := upstream/firmware
P := platform
B := build
INC := -I$(P) -I$(P)/pico_stubs -I$(U) -I$(U)/include -I$(U)/bsp
GAME_SRCS := $(U)/game.c $(U)/generated_events.c $(U)/ui.c $(U)/docking.c $(U)/crew.c \
             $(U)/asteroids.c $(U)/nebulagame.c $(U)/multiplayer.c $(U)/frontier.c $(U)/mazegame.c $(U)/contracts.c $(U)/frontier_img.c $(U)/minigames.c $(U)/touch.c \
             $(U)/bsp/base64.c $(U)/bsp/bsp_gost28147.c $(U)/bsp/bsp_komihash.c \
             $(U)/bsp/buttons.c $(U)/bsp/common.c $(U)/bsp/config.c $(U)/bsp/console_ui.c \
             $(U)/bsp/eeprom.c $(U)/bsp/framebuffer.c $(U)/bsp/terminal.c $(U)/bsp/ws2812.c
GAME_OBJS := $(patsubst %.c,$(B)/%.o,$(GAME_SRCS))
CFLAGS_GAME := -std=gnu11 -O2 -w -fPIC -DWAYFARER_HOST $(INC)
CFLAGS_HOST := -std=c11 -O2 -Wall -Wextra -fPIC $(INC)

all: $(B)/libwayfarer.so
cli: $(B)/emu-cli

$(B)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_GAME) -c $< -o $@

$(B)/Wayfarer.o: $(U)/Wayfarer.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_GAME) -Dmain=wayfarer_main -c $< -o $@

$(B)/platform_2026.o: $(P)/platform_2026.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_HOST) -c $< -o $@

$(B)/cli_2026.o: $(P)/cli_2026.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_HOST) -c $< -o $@

$(B)/host_extra.o: tools/host_extra.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_HOST) -c $< -o $@

$(B)/libwayfarer.so: $(B)/host_extra.o $(B)/platform_2026.o $(B)/Wayfarer.o $(GAME_OBJS)
	$(CC) -shared -O2 $^ -lm -o $@

$(B)/emu-cli: $(B)/cli_2026.o $(B)/platform_2026.o $(B)/Wayfarer.o $(GAME_OBJS)
	$(CC) -O2 $^ -lz -lm -o $@

PYTHON ?= $(if $(wildcard ../.venv/bin/python),$(abspath ../.venv/bin/python),python3)
PACKS := $(wildcard $(U)/packs/*.yml)
events: $(U)/generated_events.c
$(U)/generated_events.c: $(U)/events.yml $(U)/generate_events.py $(PACKS)
	cd $(U) && $(PYTHON) generate_events.py events.yml generated_events.c $(foreach p,$(PACKS),--pack packs/$(notdir $(p)))

badge:
	@test -n "$$PICO_SDK_PATH" || { echo "source ~/badge/env.sh first"; exit 1; }
	cmake -S $(U) -B $(B)/badge -DPICO_BOARD=pico2 -DPICO_SDK_PATH=$$PICO_SDK_PATH >/dev/null
	cmake --build $(B)/badge -j8
	@ls -la $(B)/badge/Wayfarer.uf2

clean:
	rm -rf $(B)

.PHONY: all cli events badge clean
