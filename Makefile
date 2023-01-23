
PROG = ultraheat
HDR = ultraheat_io.h ultraheat_parser.h ultraheat_message.h ultraheat_states.h
OTHER_SRC = ultraheat.yaml secrets.yaml

PROG_OTA = ultraheat_ota

CHECK_PROG = ultraheat_check
CHECK_SRC = ultraheat_check.cpp

$(PROG): Makefile $(HDR) $(OTHER_SRC)
	esphome compile $(PROG).yaml

$(PROG_OTA): Makefile $(PROG_OTA).h
	esphome compile $(PROG_OTA).yaml

$(CHECK_PROG): Makefile $(HDR) $(CHECK_SRC)
	g++ $(CHECK_SRC) -o $(CHECK_PROG)
	./$(CHECK_PROG)

PHONY: install
install: $(PROG)
	esphome upload ultraheat.yaml

PHONY: ota
ota: $(PROG_OTA)
	esphome compile $(PROG_OTA).yaml
	esphome upload $(PROG_OTA).yaml

PHONY: check
check: $(CHECK_PROG)

