CXX=gcc
SRC_DIR=src
LIB_DIR=lib
BUILD_DIR=build

ifeq ($(OS),Windows_NT)
  BINARY=build/dav.exe
  PLATFORM_CFLAGS=-DWIN32 -D_WIN32
  PLATFORM_LDFLAGS=-lportaudio -lm
else
  BINARY=build/dav
  PLATFORM_CFLAGS=-D__linux__
  PLATFORM_LDFLAGS=-lwayland-client -lX11 -lXfixes
endif

GDB=gdb
MKDIR=mkdir -p
RM=rm -rf

ifeq ($(OS),Windows_NT)
  GTK_CFLAGS=-I/ucrt64/include/gtk-3.0 -I/ucrt64/include/pango-1.0 -I/ucrt64/include -I/ucrt64/include/glib-2.0 -I/ucrt64/lib/glib-2.0/include -I/ucrt64/include/harfbuzz -I/ucrt64/include/freetype2 -I/ucrt64/include/libpng16 -I/ucrt64/include/fribidi -I/ucrt64/include/cairo -I/ucrt64/include/pixman-1 -I/ucrt64/include/gdk-pixbuf-2.0 -I/ucrt64/include/atk-1.0
  GTK_LIBS=-L/ucrt64/lib -lgtk-3 -lgdk-3 -lgdi32 -limm32 -lshell32 -lole32 -lwinmm -lpangocairo-1.0 -lpango-1.0 -lharfbuzz -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0
else
  GTK_CFLAGS=$(shell pkg-config --cflags gtk+-3.0 gtk-layer-shell-0)
  GTK_LIBS=$(shell pkg-config --libs gtk+-3.0 gtk-layer-shell-0) -lm -lportaudio
endif

LDFLAGS=$(GTK_LIBS) $(PLATFORM_LDFLAGS)
CXXFLAGS=-Wall -I $(LIB_DIR) $(GTK_CFLAGS) $(PLATFORM_CFLAGS) -O3 -MD -MP

ALL_SOURCES=$(wildcard $(SRC_DIR)/*.c)

ifeq ($(OS),Windows_NT)
  SOURCES=$(filter-out $(SRC_DIR)/wayland.c $(SRC_DIR)/x11.c, $(ALL_SOURCES))
else
  SOURCES=$(filter-out $(SRC_DIR)/win32_overlay.c, $(ALL_SOURCES))
endif

OBJECTS=$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

.PHONY: all run debug clean
all: compile

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $< to $@"
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
