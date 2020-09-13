#include <emu/file/nesrom.hpp>
#define DEBUG
#include <emu/utils/debug.hpp>

namespace File {

enum ErrID : int {
    ERRID_SUCCESS = 0, ERRID_INVFORMAT, ERRID_NES20, ERRID_FNOTFOUND,
};

static std::string rom_errmsg[] = {
    "no errors", "invalid NES format", "NES 2.0 not yet supported", "can't open rom file",
};



/* NOTE: private functions */
bool ROM::parseheader()
{
    read(HEADER_LEN, header);
    fformat = Format::INVALID;
    if (header[0] == 'N' && header[1] == 'E' && header[2] == 'S' && header[3] == 0x1A)
        fformat = Format::INES;
    if (fformat == Format::INES && (header[7] & 0xC) == 0x8) {
        //fformat = Format::NES20;
        debugmsg = ERRID_NES20;
        return false;
    }
    if (fformat == Format::INVALID) {
        debugmsg = ERRID_INVFORMAT;
        return false;
    }

    prgrom_size = header[4];
    chrrom_size = header[5];

    nametab_mirroring   = header[6] & 1;
    has_battery         = header[6] & 2;
    has_trainer         = header[6] & 4;
    has_fourscreenmode  = header[6] & 8;

    console_type = header[7] & 3;

    mapper = (header[6] & 0xF0) >> 4;
    mapper |= (header[7] & 0xF0);

    if (fformat == Format::INES)
        parse_ines();
    else if (fformat == Format::NES20)
        parse_nes20();
    return true;
}

void ROM::parse_ines()
{
    if (chrrom_size == 0)
        has_chrram = true;
    prgram_size = header[8];
    // the specification says this bit exists, but no emus make use of it
    //region = header[9] & 1;
    region = (header[10] & 3);
    has_prgram = header[10] & 0x10;
    has_bus_conflicts = header[10] & 0x20;
}

void ROM::parse_nes20()
{
    int shift;

    prgrom_size |= (header[9] & 0xF) << 8;
    chrrom_size |= (header[9] & 0xF0) << 8;
    mapper      |= (header[8] & 0xF) << 8;
    submapper   = header[8] & 0xF0;

    shift = header[10] & 0xF;
    if (shift == 0)
        has_prgram = false;
    else {
        has_prgram = true;
        prgram_size = 64 << shift;
    }

    shift = header[10] & 0xF0;
    eeprom_size = (shift == 0) ? 0 : 64 << shift;

    shift = header[11] & 0xF;
    if (shift == 0)
        has_chrram = false;
    else {
        has_chrram = true;
        chrram_size = 64 << shift;
    }
    shift = header[11] & 0xF0;
    chrnvram_size = (shift == 0) ? 0 : 64 << shift;

    cpu_ppu_timing =  header[12] & 3;

    vs_ppu_type = vs_hw_type = 0;
    if (console_type == CONSOLE_TYPE_VSSYSTEM) {
        vs_ppu_type = header[13] & 0xF;
        vs_hw_type = header[13] & 0xF0;
    } else if (console_type == 3)
        console_type = header[13] & 0xF;

    misc_roms_num = header[14] & 3;

        def_expansion_dev = header[15] & 0x3F;
}



/* NOTE: public functions */
bool ROM::open(const std::string &fname)
{
    romfile = std::fopen(fname.c_str(), "rb");
    if (!romfile) {
        debugmsg = ERRID_FNOTFOUND;
        return false;
    }
    filename = fname;

    if (!parseheader())
        return false;

    // get trainer
    if (has_trainer)
        read(TRAINER_LEN, trainer);
    // allocate program ROM and character ROM
    if (!has_prgram) {
        prgrom = new uint8_t[prgrom_size*16384];
        read(prgrom_size*16384, prgrom);
    }
    if (!has_chrram) {
        chrrom = new uint8_t[chrrom_size*8192];
        read(chrrom_size*8192, chrrom);
    }

    return true;
}

void ROM::close()
{
    if (romfile)
        fclose(romfile);
    if (prgrom)
        delete[] prgrom;
    if (chrrom)
        delete[] chrrom;
}

void ROM::printinfo(FILE *logfile)
{
    if (!logfile)
        return;
    std::fprintf(logfile, "%s: ", filename.c_str());
    if (fformat == Format::INES)
        std::fprintf(logfile, "iNES");
    else
        std::fprintf(logfile, "NES 2.0");
    std::fprintf(logfile, ", mapper %d, %dx16k PRG ROM, %dx8k CHR ROM",
            mapper, prgrom_size, chrrom_size);
    if (has_prgram)
        std::fprintf(logfile, ", %d PRG RAM", prgram_size);
    if (has_chrram)
        std::fprintf(logfile, ", %d CHR RAM", chrram_size);
    if (eeprom_size != 0)
        std::fprintf(logfile, ", %d EEPROM", eeprom_size);
    if (chrnvram_size != 0)
        std::fprintf(logfile, ", %d CHR NVRAM", chrnvram_size);

    if (nametab_mirroring == NAMETAB_HORZ)
        std::fprintf(logfile, ", H-Mirror");
    else
        std::fprintf(logfile, ", V-Mirror");
    if (has_battery)
        std::fprintf(logfile, ", SRAM enabled");
    if (has_trainer)
        std::fprintf(logfile, ", Trainer enabled");
    if (has_fourscreenmode)
        std::fprintf(logfile, ", Four screen mode enabled");
    std::puts("");
}

std::string &ROM::geterr()
{
    return rom_errmsg[debugmsg];
}

} // namespace nesrom

