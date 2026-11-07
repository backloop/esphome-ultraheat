
SRC_DIR = src

PROG = ultraheat
HDR = $(addprefix $(SRC_DIR)/, ultraheat_io.h ultraheat_parser.h ultraheat_message.h ultraheat_states.h statemachine.h)
PROG_HDR = $(addprefix $(SRC_DIR)/, ultraheat.h ultraheat_debug.h)
OTHER_SRC = ultraheat.yaml secrets.yaml

PROG_OTA = ultraheat_ota

CHECK_PROG = ultraheat_check
CHECK_SRC = $(SRC_DIR)/ultraheat_check.cpp

# esphome is run from its docker image by default. Use a local install with:
#   make USE_DOCKER=0
USE_DOCKER ?= 1
ESPHOME_VERSION = 2022.12.8
ESPHOME_IMAGE = esphome/esphome:$(ESPHOME_VERSION)

ifeq ($(USE_DOCKER),1)
# Runs as root since the image's bundled /piolibs is not readable by other
# users; FIX_OWNER hands the generated files back to the invoking user.
# Host networking lets OTA uploads and mDNS reach the device.
ESPHOME = docker run --rm --network host -v $(CURDIR):/config $(ESPHOME_IMAGE)
FIX_OWNER = docker run --rm -v $(CURDIR):/config --entrypoint chown $(ESPHOME_IMAGE) -R $(shell id -u):$(shell id -g) /config/.esphome
# PlatformIO keeps its packages under /config/.esphome/platformio in the image
TOOLCHAIN = .esphome/platformio/packages/toolchain-xtensa/bin
else
ESPHOME = esphome
FIX_OWNER = true
TOOLCHAIN = $(HOME)/.platformio/packages/toolchain-xtensa/bin
endif

ELF = .esphome/build/ultraheat/.pioenvs/ultraheat/firmware.elf

$(PROG): Makefile $(HDR) $(PROG_HDR) $(OTHER_SRC)
	$(ESPHOME) compile $(PROG).yaml || { $(FIX_OWNER); exit 1; }
	$(FIX_OWNER)
	#$(ESPHOME) -v compile $(PROG).yaml
	# map file is generated through platformio_options: build_flags in ultraheat.yaml
	# TODO: generate list file for main.cpp.o with interleaved sources?

	$(TOOLCHAIN)/xtensa-lx106-elf-objdump --disassemble-all --demangle $(ELF) > firmware.list

	$(TOOLCHAIN)/xtensa-lx106-elf-nm --demangle --print-size --size-sort --radix=d --line-numbers $(ELF) > firmware.sizes

$(PROG_OTA): Makefile $(SRC_DIR)/$(PROG_OTA).h $(PROG_OTA).yaml
	$(ESPHOME) compile $(PROG_OTA).yaml || { $(FIX_OWNER); exit 1; }
	$(FIX_OWNER)

$(CHECK_PROG): Makefile $(HDR) $(CHECK_SRC)
	g++ -Wall -Werror -Wpedantic $(CHECK_SRC) -o $(CHECK_PROG)

.PHONY: install
install: $(PROG)
	$(ESPHOME) upload $(PROG).yaml

.PHONY: ota
ota: $(PROG_OTA)
	$(ESPHOME) upload $(PROG_OTA).yaml

.PHONY: check
check: $(CHECK_PROG)
	./$(CHECK_PROG)

.PHONY: clean
clean:
	rm -rf $(CHECK_PROG)
