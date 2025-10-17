CXX=gcc
GDB=gdb
MKDIR=mkdir -p
RM=rm -rf

SRC_DIR=src
LIB_DIR=lib
BUILD_DIR=build

ALL_SOURCES=$(wildcard $(SRC_DIR)/*.c)

PLATFORM_LDFLAGS=-lportaudio -lm
GTK_CFLAGS=$(shell pkg-config --cflags gtk+-3.0)
GTK_LIBS=$(shell pkg-config --libs gtk+-3.0)

ifeq ($(OS),Windows_NT)
  BINARY=build/dav.exe
  PLATFORM_CFLAGS=-DWIN32 -D_WIN32
  SOURCES=$(filter-out $(SRC_DIR)/wayland.c $(SRC_DIR)/x11.c, $(ALL_SOURCES))
else
  BINARY=build/dav
  PLATFORM_CFLAGS=-D__linux__
  PLATFORM_LDFLAGS+=-lwayland-client -lX11 -lXfixes
  GTK_CFLAGS+=$(shell pkg-config --cflags gtk-layer-shell-0)
  GTK_LIBS+=$(shell pkg-config --libs gtk-layer-shell-0)
  SOURCES=$(filter-out $(SRC_DIR)/win32_overlay.c, $(ALL_SOURCES))
endif

LDFLAGS=$(GTK_LIBS) $(PLATFORM_LDFLAGS)
CXXFLAGS=-Wall -I $(LIB_DIR) $(GTK_CFLAGS) $(PLATFORM_CFLAGS) -O3 -MD -MP
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