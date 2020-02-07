CXX = g++
CXXFLAGS = -Werror -Wextra -Wall -pedantic -std=c++17 -Ofast -g

.PHONY: clean

SRC = src/main.cc src/converter/converter.cc src/option/option.cc
TARGET = cmkv


all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -lfreeimageplus -o $(TARGET)

clean:
	rm -rf $(TARGET)
