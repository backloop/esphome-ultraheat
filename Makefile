
PROG = ultraheat
HDR = ultraheat_io.h ultraheat_parser.h ultraheat_message.h ultraheat_states.h statemachine.h
OTHER_SRC = ultraheat.yaml secrets.yaml

PROG_OTA = ultraheat_ota

CHECK_PROG = ultraheat_check
CHECK_SRC = ultraheat_check.cpp

$(PROG): Makefile $(HDR) $(OTHER_SRC)
	esphome compile $(PROG).yaml
	#esphome -v compile $(PROG).yaml
	# map file is generated through platformio_options: build_flags in ultraheat.yaml
	# TODO: generate list file for main.cpp.o with interleaved sources?

	/home/backloop/.platformio/packages/toolchain-xtensa/bin/xtensa-lx106-elf-objdump --disassemble-all --demangle ./.esphome/build/ultraheat/.pioenvs/ultraheat/firmware.elf > firmware.list

	/home/backloop/.platformio/packages/toolchain-xtensa/bin/xtensa-lx106-elf-nm --demangle --print-size --size-sort --radix=d --line-numbers ./.esphome/build/ultraheat/.pioenvs/ultraheat/firmware.elf > firmware.sizes

$(PROG_OTA): Makefile $(PROG_OTA).h
	esphome compile $(PROG_OTA).yaml

$(CHECK_PROG): Makefile $(HDR) $(CHECK_SRC)
	g++ -Wall -Werror -Wpedantic $(CHECK_SRC) -o $(CHECK_PROG)

PHONY: install
install: $(PROG)
	esphome upload $(PROG).yaml

PHONY: ota
ota: $(PROG_OTA)
	esphome compile $(PROG_OTA).yaml
	esphome upload $(PROG_OTA).yaml

PHONY: check
check: $(CHECK_PROG)
	./$(CHECK_PROG)

PHONY: clean
clean:
	rm -rf $(CHECK_PROG)
