VPATH=emu:emu/core:emu/util:emu/io:emu/video

HEADERS = cartridge.hpp cpu.hpp memorymap.hpp types.hpp ppu.hpp \
		  genericbus.hpp \
		  cmdline.hpp file.hpp debug.hpp stringops.hpp \
		  video.hpp emulator.hpp

OBJS = main.o emulator.o \
	   cpu.o cartridge.o ppu.o genericbus.o \
	   file.o cmdline.o stringops.o \
	   video.o

LIBS = -lm -lSDL2

CXX = g++
CFLAGS = -I. -std=c++17 -Wall -Wextra -pipe \
		 -Wcast-align -Wcast-qual -Wpointer-arith \
		 -Wformat=2 -Wmissing-include-dirs -Wno-unused-parameter

DEBDIR = debug
DEBPRGNAME = emu
DEBCFLAGS = -g
DEBOBJS = $(patsubst %,$(DEBDIR)/%,$(OBJS))
RELDIR = release
RELPRGNAME = emu-release

all: debug

# these are special - they #include other .cpp files
$(DEBDIR)/cpu.o: emu/core/cpu.cpp emu/core/opcodes.cpp emu/core/disassemble.cpp $(HEADERS)
	$(CXX) $(CFLAGS) -c $< -o $@

$(DEBDIR)/ppu.o: emu/core/ppu.cpp emu/core/ppumain.cpp emu/core/vram.cpp emu/core/background.cpp emu/core/oam.cpp $(HEADERS)
	$(CXX) $(CFLAGS) -c $< -o $@

$(DEBDIR)/video.o: emu/video/video.cpp emu/video/videosdl.cpp $(HEADERS)
	$(CXX) $(CFLAGS) -c $< -o $@

$(DEBDIR)/%.o: %.cpp $(HEADERS)
	$(CXX) $(CFLAGS) -c $< -o $@

directories:
	mkdir -p $(DEBDIR) $(RELDIR)

debug: CFLAGS += $(DEBCFLAGS)
debug: directories $(DEBPRGNAME)

$(DEBPRGNAME): $(DEBOBJS)
	$(CXX) $(DEBOBJS) -o $(DEBDIR)/$(DEBPRGNAME) $(LIBS)

.PHONY: clean
clean:
	rm -rf $(DEBDIR)/*.o $(DEBDIR)/$(DEBPRGNAME) $(RELDIR)/*.o $(RELDIR)/$(RELPRGNAME)

