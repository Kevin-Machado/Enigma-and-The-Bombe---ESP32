CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2 -DHOST_BUILD -Isrc
SRC := \
	src/enigma/rotor.cpp \
	src/enigma/plugboard.cpp \
	src/enigma/reflector.cpp \
	src/enigma/enigma_machine.cpp \
	src/bombe/menu.cpp \
	src/bombe/constraints.cpp \
	src/bombe/verifier.cpp \
	src/bombe/bombe_engine.cpp \
	src/hardware/leds.cpp \
	src/web/json_util.cpp \
	src/web/api.cpp \
	src/web/pages.cpp \
	src/web/server.cpp

TEST_BIN := tests/host_tests

.PHONY: test host clean

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): tests/test_runner.cpp $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ tests/test_runner.cpp $(SRC)

host: test

clean:
	rm -f $(TEST_BIN)
