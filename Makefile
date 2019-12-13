CXX = g++
CXXFLAGS = -Werror -Wextra -Wall -pedantic -std=c++17

.PHONY: clean

SRC = src/main.cc src/image/converter.cc
TARGET = cmkv



all: $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -Idist/ -Ldist/ -lfreeimage -o $(TARGET)

clean:
	rm -rf $(TARGET)