TARGET := maze_gs
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
SOURCE := main.cpp

ifeq ($(OS),Windows_NT)
    TARGET := $(TARGET).exe
    RM := del /Q
else
    RM := rm -f
endif

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) $(SOURCE) -o $(TARGET)

clean:
	$(RM) $(BUILD_DIR)

.PHONY: all clean