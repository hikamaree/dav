CC = gcc
MKDIR = mkdir -p
RM = rm -rf
PKG = pkg-config

SRC = src
LIB = lib
BUILD = build
RES = resources

MXE = /home/user/mxe
TGT = x86_64-w64-mingw32.static
MXE_CC = $(MXE)/usr/bin/$(TGT)-gcc
MXE_PKG = $(MXE)/usr/bin/$(TGT)-pkg-config
MXE_GRES = $(MXE)/usr/$(TGT)/bin/glib-compile-resources
MXE_GSCH = $(MXE)/usr/$(TGT)/bin/glib-compile-schemas
MXE_SCH = $(MXE)/usr/$(TGT)/share/glib-2.0/schemas

SRCS = $(wildcard $(SRC)/*.c)
L_SRCS = $(filter-out $(SRC)/win32_overlay.c,$(SRCS))
W_SRCS = $(filter-out $(SRC)/wayland.c $(SRC)/x11.c,$(SRCS))
L_OBJS = $(L_SRCS:$(SRC)/%.c=$(BUILD)/lin/%.o)
W_OBJS = $(W_SRCS:$(SRC)/%.c=$(BUILD)/win/%.o)

FLAGS = -Wall -I$(LIB) -O3 -MD -MP
L_CFLAGS = $(FLAGS) -D__linux__ $(shell $(PKG) --cflags gtk+-3.0 gtk-layer-shell-0)
W_CFLAGS = $(FLAGS) -DWIN32 -D_WIN32 $(shell $(MXE_PKG) --cflags gtk+-3.0)
L_LDFLAGS = $(shell $(PKG) --libs gtk+-3.0 gtk-layer-shell-0) -lportaudio -lm -lwayland-client -lX11 -lXfixes
W_LDFLAGS = $(shell $(MXE_PKG) --libs gtk+-3.0) -lportaudio -lm -lwinmm -lsetupapi -lole32

.PHONY: all windows run clean

all: $(BUILD)/lin/dav

$(BUILD)/lin/dav: $(L_OBJS)
	$(CC) $^ -o $@ $(L_LDFLAGS)

$(BUILD)/lin/%.o: $(SRC)/%.c | $(BUILD)/lin
	$(CC) $(L_CFLAGS) -c $< -o $@

$(BUILD)/lin:
	@$(MKDIR) $@

windows: clean
	@$(MAKE) --no-print-directory $(BUILD)/win/dav.exe

$(BUILD)/win/dav.exe: $(RES)/schemas/gschemas.compiled $(W_OBJS) $(BUILD)/win/resources.o
	$(MXE_CC) $(filter %.o,$^) -o $@ $(W_LDFLAGS)

$(BUILD)/win/%.o: $(SRC)/%.c | $(BUILD)/win
	$(MXE_CC) $(W_CFLAGS) -c $< -o $@

$(BUILD)/win:
	@$(MKDIR) $@

$(BUILD)/win/resources.o: $(BUILD)/win/resources.c
	$(MXE_CC) $(W_CFLAGS) -c $< -o $@

$(BUILD)/win/resources.c: resources.gresource.xml $(RES)/schemas/gschemas.compiled
	$(MXE_GRES) --target=$@ --generate-source --sourcedir=$(RES) $<

$(RES)/schemas/gschemas.compiled:
	@$(MKDIR) $(RES)/schemas
	@cp $(MXE_SCH)/*.xml $(RES)/schemas/ 2>/dev/null || true
	@$(MXE_GSCH) $(RES)/schemas

run: all
	./$(BUILD)/lin/dav

clean:
	@$(RM) $(BUILD) $(RES)

-include $(BUILD)/*.d