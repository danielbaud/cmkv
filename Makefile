CXX = g++
CXXFLAGS = -Werror -Wextra -Wall -pedantic -std=c++17 -O5

.PHONY: clean

SRC = src/main.cc src/converter/converter.cc
TARGET = cmkv



all: $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -lfreeimageplus -o $(TARGET)

clean:
	rm -rf $(TARGET)