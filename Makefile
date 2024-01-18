SRC_DIR=src
LIB_DIR=lib
BUILD_DIR=build
BINARY=build/audio

CXX=gcc
GDB=gdb
MKDIR=mkdir -p
RM=rm -rf

GTKFLAGS=$(shell pkg-config --cflags --libs gtk+-3.0)
LDFLAGS=-lraylib $(GTKFLAGS) -lm -lportaudio
CXXFLAGS=-Wall -I $(LIB_DIR) $(GTKFLAGS) -O3 -MD -MP

SOURCES=$(wildcard $(SRC_DIR)/*.c)
OBJECTS=$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))
.PHONY: all run debug clean
all: compile

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BINARY): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

compile:
	$(MKDIR) $(BUILD_DIR)
	$(MAKE) $(BINARY)

run: compile
	./$(BINARY)

clean:
	$(RM) $(BUILD_DIR)

-include $(OBJECTS:.o=.d)
