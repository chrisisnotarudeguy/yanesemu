#include <emu/emulator.hpp>

#include <string_view>
#include <emu/util/unsigned.hpp>
#include <emu/util/debug.hpp>
#include <emu/util/file.hpp>

using namespace Core;

void Emulator::run()
{
    cpu.run();
    int curr_cycle = cpu.get_cycles();
    int cycle_diff = curr_cycle - cycle;
    // run 3 ppu cycles for 1 cpu cycle
    for (int i = 0; i < cycle_diff*3; i++)
        ppu.run();
    cycle = curr_cycle;
}

void Emulator::log(Util::File &logfile)
{
    if (!logfile)
        return;
    logfile.putstr(cpu.get_info() + ' ');
    logfile.putstr(ppu.get_info() + ' ');
    logfile.print("Instruction [{:02X}] ", cpu.peek_opcode());
    logfile.putstr(cpu.disassemble());
    logfile.putc('\n');
}

void Emulator::dump(Util::File &dumpfile)
{
    auto dump_mem = [&](auto &mem) {
        for (std::size_t i = 0; i < mem.size(); i++) {
            dumpfile.print("{:04X}: ", i);
            for (std::size_t j = 0; j < 16; j++)
                dumpfile.print("{:02X} ", mem[i++]);
            dumpfile.putc('\n');
        }
        dumpfile.putc('\n');
    };
    dump_mem(cpu.get_memory());
    // dump_to(dumpfile, ppu.getmemory(), PPUMap::MEMSIZE);
}

void Emulator::run_frame(Util::File &logfile)
{
    while (!nmi) {
        log(logfile);
        run();
    }
    nmi = false;
}

