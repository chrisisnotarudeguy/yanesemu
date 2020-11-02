#ifndef INSIDE_CPU_CPP
#error "Only emu/core/cpu.cpp may #include this file."
#else

inline static std::string disass_implied(const char name[4])
{
    return std::string(name);
}

inline static std::string disass_imm(const char name[4], uint8_t op)
{
    return Utils::strprintf("%s #$%02X", name, op);
}

inline static std::string disass_accum(const char name[4])
{
    return Utils::strprintf("%s A", name);
}

inline static std::string disass_zero(const char name[4], uint8_t op)
{
    return Utils::strprintf("%s $%02X", name, op);
}

inline static std::string disass_zerox(const char name[4], uint8_t op)
{
    return Utils::strprintf("%s $%02X,x", name, op);
}

inline static std::string disass_zeroy(const char name[4], uint8_t op)
{
    return Utils::strprintf("%s $%02X,y", name, op);
}

inline static std::string disass_abs(const char name[4], uint8_t low, uint8_t hi)
{
    return Utils::strprintf("%s $%02X%02X", name, hi, low);
}

inline static std::string disass_absx(const char name[4], uint8_t low, uint8_t hi)
{
    return Utils::strprintf("%s $%02X%02X,x", name, hi, low);
}

inline static std::string disass_absy(const char name[4], uint8_t low, uint8_t hi)
{
    return Utils::strprintf("%s $%02X%02X,y", name, hi, low);
}

inline static std::string disass_ind(const char name[4], uint8_t low, uint8_t hi)
{
    return Utils::strprintf("%s ($%02X%02X)", name, hi, low);
}

inline static std::string disass_indx(const char name[4], uint8_t op)
{
    return Utils::strprintf("%s ($%02X,x)", name, op);
}

inline static std::string disass_indy(const char name[4], uint8_t op)
{
    return Utils::strprintf("%s ($%02X),y", name, op);
}

inline static std::string disass_branch(const char name[4], int8_t disp,
        uint16_t addr, bool took)
{
    return Utils::strprintf("%s %d [$%04X] %s", name, disp, addr,
            (took) ? "[Branch taken]" : "[Branch not taken]");
}

std::string CPU::disassemble() const
{
    uint8_t instruction = bus.memory[pc.reg];
    uint8_t op1 = bus.memory[pc.reg+1];
    uint8_t op2 = bus.memory[pc.reg+2];
#define INSTR_IMPLD(id, name) \
    case id: return disass_implied(#name);
#define INSTR_AMODE(id, name, mode, ...) \
    case id: return disass_##mode(#name, __VA_ARGS__);
#define INSTR_BRNCH(id, name, expr) \
    case id: return disass_branch(#name, (int8_t) op1, pc.reg + 2 + (int8_t) op1, expr);
#define INSTR_ACCUM(id, name) \
    case id: return disass_accum(#name);

    switch(instruction) {
        INSTR_IMPLD(0x00, BRK)
        INSTR_AMODE(0x01, ORA, indx, op1)
        INSTR_AMODE(0x05, ORA, zero, op1)
        INSTR_AMODE(0x06, ASL, zero, op1)
        INSTR_IMPLD(0x08, PHP)
        INSTR_AMODE(0x09, ORA, imm, op1)
        INSTR_ACCUM(0x0A, ASL)
        INSTR_AMODE(0x0D, ORA, abs, op1, op2)
        INSTR_AMODE(0x0E, ASL, abs, op1, op2)
        INSTR_BRNCH(0x10, BPL, procstatus.neg == 0)
        INSTR_AMODE(0x11, ORA, indy, op1)
        INSTR_AMODE(0x15, ORA, zerox, op1)
        INSTR_AMODE(0x16, ASL, zerox, op1)
        INSTR_IMPLD(0x18, CLC)
        INSTR_AMODE(0x19, ORA, absy, op1, op2)
        INSTR_AMODE(0x1D, ORA, absx, op1, op2)
        INSTR_AMODE(0x1E, ASL, absx, op1, op2)
        INSTR_AMODE(0x20, JSR, abs, op1, op2)
        INSTR_AMODE(0x21, AND, indx, op1)
        INSTR_AMODE(0x24, BIT, zero, op1)
        INSTR_AMODE(0x25, AND, zero, op1)
        INSTR_AMODE(0x26, ROL, zero, op1)
        INSTR_IMPLD(0x28, PLP)
        INSTR_AMODE(0x29, AND, imm, op1)
        INSTR_ACCUM(0x2A, ROL)
        INSTR_AMODE(0x2C, BIT, abs, op1, op2)
        INSTR_AMODE(0x2D, AND, abs, op1, op2)
        INSTR_AMODE(0x2E, ROL, abs, op1, op2)
        INSTR_BRNCH(0x30, BMI, procstatus.neg == 1)
        INSTR_AMODE(0x31, AND, indy, op1)
        INSTR_AMODE(0x35, AND, zerox, op1)
        INSTR_AMODE(0x36, ROL, zerox, op1)
        INSTR_IMPLD(0x38, SEC)
        INSTR_AMODE(0x39, AND, absy, op1, op2)
        INSTR_AMODE(0x3D, AND, absx, op1, op2)
        INSTR_AMODE(0x3E, ROL, absx, op1, op2)
        INSTR_IMPLD(0x40, RTI)
        INSTR_AMODE(0x41, EOR, indx, op1)
        INSTR_AMODE(0x45, EOR, zero, op1)
        INSTR_AMODE(0x46, LSR, zero, op1)
        INSTR_IMPLD(0x48, PHA)
        INSTR_AMODE(0x49, EOR, imm, op1)
        INSTR_ACCUM(0x4A, LSR)
        INSTR_AMODE(0x4C, JMP, abs, op1, op2)
        INSTR_AMODE(0x4D, EOR, abs, op1, op2)
        INSTR_AMODE(0x4E, LSR, abs, op1, op2)
        INSTR_BRNCH(0x50, BVC, procstatus.ov == 0)
        INSTR_AMODE(0x51, EOR, indy, op1)
        INSTR_AMODE(0x55, EOR, zerox, op1)
        INSTR_AMODE(0x56, LSR, zerox, op1)
        INSTR_IMPLD(0x58, CLI)
        INSTR_AMODE(0x59, EOR, absy, op1, op2)
        INSTR_AMODE(0x5D, EOR, absx, op1, op2)
        INSTR_AMODE(0x5E, LSR, absx, op1, op2)
        INSTR_IMPLD(0x60, RTS)
        INSTR_AMODE(0x61, ADC, indx, op1)
        INSTR_AMODE(0x65, ADC, zero, op1)
        INSTR_AMODE(0x66, ROR, zero, op1)
        INSTR_IMPLD(0x68, PLA)
        INSTR_AMODE(0x69, ADC, imm, op1)
        INSTR_ACCUM(0x6A, ROR)
        INSTR_AMODE(0x6C, JMP, ind, op1, op2)
        INSTR_AMODE(0x6D, ADC, abs, op1, op2)
        INSTR_AMODE(0x6E, ROR, abs, op1, op2)
        INSTR_BRNCH(0x70, BVS, procstatus.ov == 1)
        INSTR_AMODE(0x71, ADC, indy, op1)
        INSTR_AMODE(0x75, ADC, zerox, op1)
        INSTR_AMODE(0x76, ROR, zerox, op1)
        INSTR_IMPLD(0x78, SEI)
        INSTR_AMODE(0x79, ADC, absy, op1, op2)
        INSTR_AMODE(0x7D, ADC, absx, op1, op2)
        INSTR_AMODE(0x7E, ROR, absx, op1, op2)
        INSTR_AMODE(0x81, STA, indx, op1)
        INSTR_AMODE(0x84, STY, zero, op1)
        INSTR_AMODE(0x85, STA, zero, op1)
        INSTR_AMODE(0x86, STX, zero, op1)
        INSTR_IMPLD(0x88, DEY)
        INSTR_IMPLD(0x8A, TXA)
        INSTR_AMODE(0x8C, STY, abs, op1, op2)
        INSTR_AMODE(0x8D, STA, abs, op1, op2)
        INSTR_AMODE(0x8E, STX, abs, op1, op2)
        INSTR_BRNCH(0x90, BCC, procstatus.carry == 0)
        INSTR_AMODE(0x91, STA, indy, op1)
        INSTR_AMODE(0x94, STA, zerox, op1)
        INSTR_AMODE(0x95, STA, zerox, op1)
        INSTR_AMODE(0x96, STX, zeroy, op1)
        INSTR_IMPLD(0x98, TYA)
        INSTR_AMODE(0x99, STA, absy, op1, op2)
        INSTR_IMPLD(0x9A, TXS)
        INSTR_AMODE(0x9D, STA, absx, op1, op2)
        INSTR_AMODE(0xA0, LDY, imm, op1)
        INSTR_AMODE(0xA1, LDA, indx, op1)
        INSTR_AMODE(0xA2, LDX, imm, op1)
        INSTR_AMODE(0xA4, LDY, zero, op1)
        INSTR_AMODE(0xA5, LDA, zero, op1)
        INSTR_AMODE(0xA6, LDX, zero, op1)
        INSTR_IMPLD(0xA8, TAY)
        INSTR_AMODE(0xA9, LDA, imm, op1)
        INSTR_IMPLD(0xAA, TAX)
        INSTR_AMODE(0xAC, LDY, abs, op1, op2)
        INSTR_AMODE(0xAD, LDA, abs, op1, op2)
        INSTR_AMODE(0xAE, LDX, abs, op1, op2)
        INSTR_BRNCH(0xB0, BCS, procstatus.carry == 1)
        INSTR_AMODE(0xB1, LDA, indy, op1)
        INSTR_AMODE(0xB4, LDY, zerox, op1)
        INSTR_AMODE(0xB5, LDA, zerox, op1)
        INSTR_AMODE(0xB6, LDX, zeroy, op1)
        INSTR_IMPLD(0xB8, CLV)
        INSTR_AMODE(0xB9, LDA, absy, op1, op2)
        INSTR_IMPLD(0xBA, TSX)
        INSTR_AMODE(0xBC, LDY, absx, op1, op2)
        INSTR_AMODE(0xBD, LDA, absx, op1, op2)
        INSTR_AMODE(0xBE, LDX, absy, op1, op2)
        INSTR_AMODE(0xC0, CPY, imm, op1)
        INSTR_AMODE(0xC1, CMP, indx, op1)
        INSTR_AMODE(0xC4, CPY, zero, op1)
        INSTR_AMODE(0xC5, CMP, zero, op1)
        INSTR_AMODE(0xC6, DEC, zero, op1)
        INSTR_IMPLD(0xC8, INY)
        INSTR_AMODE(0xC9, CMP, imm, op1)
        INSTR_IMPLD(0xCA, DEX)
        INSTR_AMODE(0xCC, CPY, abs, op1, op2)
        INSTR_AMODE(0xCD, CMP, abs, op1, op2)
        INSTR_AMODE(0xCE, DEC, abs, op1, op2)
        INSTR_BRNCH(0xD0, BNE, procstatus.zero == 0)
        INSTR_AMODE(0xD1, CMP, indy, op1)
        INSTR_AMODE(0xD5, CMP, zerox, op1)
        INSTR_AMODE(0xD6, DEC, zerox, op1)
        INSTR_IMPLD(0xD8, CLD)
        INSTR_AMODE(0xD9, CMP, absy, op1, op2)
        INSTR_AMODE(0xDD, CMP, absx, op1, op2)
        INSTR_AMODE(0xDE, DEC, absx, op1, op2)
        INSTR_AMODE(0xE0, CPX, imm, op1)
        INSTR_AMODE(0xE1, SBC, indx, op1)
        INSTR_AMODE(0xE4, CPX, zero, op1)
        INSTR_AMODE(0xE5, SBC, zero, op1)
        INSTR_AMODE(0xE6, INC, zero, op1)
        INSTR_IMPLD(0xE8, INX)
        INSTR_AMODE(0xE9, SBC, imm, op1)
        INSTR_IMPLD(0xEA, NOP)
        INSTR_AMODE(0xEC, CPX, abs, op1, op2)
        INSTR_AMODE(0xED, SBC, abs, op1, op2)
        INSTR_AMODE(0xEE, INC, abs, op1, op2)
        INSTR_BRNCH(0xF0, BEQ, procstatus.zero == 1)
        INSTR_AMODE(0xF1, SBC, indy, op1)
        INSTR_AMODE(0xF5, SBC, zerox, op1)
        INSTR_AMODE(0xF6, INC, zerox, op1)
        INSTR_AMODE(0xF9, SBC, absy, op1, op2)
        INSTR_AMODE(0xFD, SBC, absx, op1, op2)
        INSTR_AMODE(0xFE, INC, absx, op1, op2)
        default:
            return "[Unknown]";
    }

#undef INSTR_IMPLD
#undef INSTR_AMODE
#undef INSTR_AMODE
#undef INSTR_IMPL
}

#endif
