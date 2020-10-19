VPATH=emu:emu/core:emu/utils:emu/io:emu/video

HEADERS = bus.hpp cpu.hpp memorymap.hpp ppu.hpp types.hpp ppubus.hpp \
		  nesrom.hpp file.hpp \
		  cmdargs.hpp debug.hpp stringops.hpp \
		  video.hpp

OBJS = main.o \
	   cpu.o bus.o ppu.o ppubus.o \
	   nesrom.o file.o \
	   cmdargs.o stringops.o \
	   video.o

LIBS = -lm -lSDL2

CXX = g++
CFLAGS = -I. -std=c++17 -Wall -Wextra -pipe \
		 -Wcast-align -Wcast-qual -Wpointer-arith \
		 -Wformat=2 -Wmissing-declarations -Wmissing-include-dirs \

DEBDIR = debug
DEBPRGNAME = emu
DEBCFLAGS = -g
DEBOBJS = $(patsubst %,$(DEBDIR)/%,$(OBJS))
RELDIR = release

all: debug

# these are special - they #include other .cpp files
$(DEBDIR)/cpu.o: emu/core/cpu.cpp emu/core/opcodes.cpp emu/core/disassemble.cpp $(HEADERS)
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
	rm -f $(DEBOBJDIR)/*.o $(DEBDIR)/$(DEBPRGNAME) $(RELOBJDIR)/*.o $(RELDIR)/$(RELPRGNAME)

