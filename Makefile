
CHECK_PROG = ultraheat_check
CHECK_SRC = ultraheat_check.cpp

PROG = ultraheat

$(PROG):
	esphome compile ultraheat.yaml

$(CHECK_PROG): $(CHECK_SRC)
	g++ $(CHECK_SRC) -o $(CHECK_PROG)
	./$(CHECK_PROG)

