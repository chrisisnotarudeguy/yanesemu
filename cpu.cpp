#include "cpu.h"

#include <cstdio>
#include "memorymap.h"

static 
uint8_t CPU::fetch()
{
    uint8_t buf[1];
    rom.read(1, buf);
    return *buf;
}

/*void decode(uint8_t byte, uint8_t &opcode, AddrMode &mode)
{
    opcode = 0;
    mode = AddrMode::IMM;
}*/

/*void CPU::execute(uint8_t opcode, AddrMode mode)
{
    uint8_t low, hi, value, tmp, sign;
    uint16_t addr;

    switch(mode) {
    case SINGLE:
        break;
    case ACCUM:
        break;
    case IMM:
        low = fetch_op();
        value = low;
        break;
    case ZERO:
        low = fetch_op();
        value = memory[low];
        break;
    case ZERO_X:
        low = fetch_op() + xreg;
        value = memory[low];
        break;
    case ABS:
        low = fetch_op();
        hi = fetch_op();
        addr = build_addr(low, hi);
        value = memory[addr];
        break;
    case ABS_X:
        low = fetch_op();
        hi = fetch_op();
        addr = low;
        addr = build_addr(low, hi);
        value = memory[addr];
        break;
    case ABS_Y:
        low = fetch_op();
        hi = fetch_op();
        addr = build_addr(low, hi);
        addr += yreg;
        value = memory[addr];
        break;
    case IND:
        low = fetch_op();
        hi = fetch_op();
        addr = build_addr(low, hi);
        addr = build_addr(memory[addr], memory[addr+1]);
        value = memory[addr];
        break;
    case IND_X:
        low = fetch_op() + xreg;
        addr = build_addr(memory[low], memory[low+1]);
        value = memory[addr];
        break;
    case IND_Y:
        low = fetch_op();
        addr = build_addr(memory[low], memory[low+1]);
        addr += yreg;
        value = memory[addr];
        break;
    default:
        std::fprintf(stderr, "error: unknown mode\n");
    }

    switch (opcode) {
    case ADC:
        sign = accum & 0x80;
        accum += value + procstatus.carry;
        procstatus.zero     = (accum == 0) ?                1 : 0;
        procstatus.neg      = ((accum & 0x80) == 1) ?       1 : 0;
        procstatus.ov       = ((accum & 0x80) != sign) ?    1 : 0;
        procstatus.carry    = (procstatus.ov == 1) ?        1 : 0;
        break;
    case AND:
        accum &= value;
        procstatus.zero     = (accum == 0) ?                1 : 0;
        procstatus.neg      = ((accum & 0x80) == 1) ?       1 : 0;
        break;
    case ASL:
        if (mode == AddrMode::ACCUM)
            accum <<= 1;
        else
            memory[addr] <<= 1;
    case BIT:
        break;
    case BPL:
        pc += low * (procstatus.neg+1);
        break;
    case BMI:
        pc += low * procstatus.neg;
        break;
    case BVC:
        pc += low * (procstatus.ov+1);
        break;
    case BVS:
        pc += low * procstatus.ov;
        break;
    case BCC:
        pc += low * (procstatus.carry+1);
        break;
    case BCS:
        pc += low * procstatus.carry;
        break;
    case BNE:
        pc += low * (procstatus.zero+1);
        break;
    case BEQ:
        pc += low * procstatus.zero;
        break;
    case BRK:
        break;
    case CMP:
        cmp(accum, value);
        break;
    case CPX:
        cmp(xreg, value);
        break;
    case CPY:
        cmp(yreg, value);
        break;
    case DEC:
        memory[addr]--;
        break;
    case DEX:
        xreg--;
        break;
    case DEY:
        yreg--;
        break;
    case EOR:
        accum ^= value;
        break;
    case CLC:
        procstatus.carry = 0;
        break;
    case SEC:
        procstatus.carry = 1;
        break;
    case CLI:
        procstatus.intdis = 1;
        break;
    case SEI:
        procstatus.intdis = 0;
        break;
    case CLV:
        procstatus.ov = 0;
        break;
    case INC:
        memory[addr] += 1;
        break;
    case JMP:
        jmp(addr);
        break;
    case JSR:
        push(pc-1);
        push(pc-2);
        jmp(addr);
        break;
    case LDA:
        accum = value;
        break;
    case LDX:
        xreg = value;
        break;
    case LDY:
        yreg = value;
        break;
    case LSR:
        if (mode == AddrMode::ACCUM) {
            procstatus.carry = accum & 1;
            accum >>= 1;
        } else {
            procstatus.carry = memory[addr] & 1;
            memory[addr] >>= 1;
        }
        break;
    case NOP:
        break;
    case ORA:
        accum |= value;
        break;
    case PHA:
        push(accum);
        break;
    case PLA:
        accum = pull();
        break;
    case PHP:
        push(procstatus.reg);
        break;
    case PLP:
        procstatus.reg = pull();
        break;
    case ROL:
        tmp = procstatus.carry;
        if (mode == AddrMode::ACCUM) {
            procstatus.carry = accum & 0x80;
            accum <<= 1;
            accum |= tmp;
        } else {
            procstatus.carry = memory[addr] & 0x80;
            memory[addr] <<= 1;
            memory[addr] |= tmp;
        }
        break;
    case ROR:
        tmp = procstatus.carry;
        if (mode == AddrMode::ACCUM) {
            procstatus.carry = accum & 1;
            accum >>= 1;
            accum |= tmp << 8;
        } else {
            procstatus.carry = memory[addr] & 1;
            memory[addr] >>= 1;
            memory[addr] |= tmp << 8;
        }
        break;
    case RTI:
        break;
    case RTS:
        addr = pull();
        addr |= pull() << 8;
        jmp(addr+1);
        break;
    case SBC:
        accum -= value;
        if (procstatus.carry == 1)
            accum--;
        break;
    case STA:
        memory[addr] = accum;
        break;
    case STX:
        memory[addr] = xreg;
        break;
    case STY:
        memory[addr] = yreg;
        break;
    case TAX:
        xreg = accum;
        break;
    case TXA:
        accum = xreg;
        break;
    case TAY:
        yreg = accum;
        break;
    case TYA:
        accum = yreg;
        break;
    case TXS:
        sp = xreg;
        break;
    case TSX:
        xreg = sp;
        break;
    default:
        std::fprintf(stderr, "error: unknown opcode: %d", opcode);
    }
}*/

#define INSTR_CASE(id, name, mode) \
    case id: instr_ ##name##_##mode(); break;

void CPU::execute(uint8_t opcode)
{
    switch(opcode) {
        INSTR_CASE(0x00, brk, impl);
        INSTR_CASE(0x01, ora, indx);
        INSTR_CASE(0x05, ora, zero);
        INSTR_CASE(0x06, asl, zero);
        INSTR_CASE(0x08, php, impl);
        INSTR_CASE(0x09, ora, imm);
        INSTR_CASE(0x0A, asl, accum);
        INSTR_CASE(0x0D, ora, abs);
        INSTR_CASE(0x0E, asl, abs);
        INSTR_CASE(0x10, bpl, impl);
        INSTR_CASE(0x11, ora, indy);
        INSTR_CASE(0x15, ora, zerox);
        INSTR_CASE(0x16, asl, zerox);
        INSTR_CASE(0x18, clc, impl);
        INSTR_CASE(0x19, ora, absy);
        INSTR_CASE(0x1D, ora, absx);
        INSTR_CASE(0x1E, asl, absx);
        INSTR_CASE(0x20, jsr, abs);
        INSTR_CASE(0x21, and, indx);
        INSTR_CASE(0x24, bit, zero);
        INSTR_CASE(0x25, and, zero);
        INSTR_CASE(0x26, rol, zero);
        INSTR_CASE(0x28, plp, impl);
        INSTR_CASE(0x29, and, imm);
        INSTR_CASE(0x2A, rol, accum);
        INSTR_CASE(0x2C, bit, abs);
        INSTR_CASE(0x2D, and, abs);
        INSTR_CASE(0x2E, rol, abs);
        INSTR_CASE(0x30, bmi, impl);
        INSTR_CASE(0x31, and, indy);
        INSTR_CASE(0x35, and, zerox);
        INSTR_CASE(0x36, rol, zerox);
        INSTR_CASE(0x38, sec, impl);
        INSTR_CASE(0x39, and, absy);
        INSTR_CASE(0x3D, and, absx);
        INSTR_CASE(0x3E, rol, absx);
        INSTR_CASE(0x40, rti, impl);
        INSTR_CASE(0x41, eor, indx);
        INSTR_CASE(0x45, eor, zero);
        INSTR_CASE(0x46, lsr, zero);
        INSTR_CASE(0x48, pha, impl);
        INSTR_CASE(0x49, eor, imm);
        INSTR_CASE(0x4A, lsr, accum);
        INSTR_CASE(0x4C, jmp, abs);
        INSTR_CASE(0x4D, eor, abs);
        INSTR_CASE(0x4E, lsr, abs);
        INSTR_CASE(0x50, bvc, impl);
        INSTR_CASE(0x51, eor, indy);
        INSTR_CASE(0x55, eor, zerox);
        INSTR_CASE(0x56, lsr, zerox);
        INSTR_CASE(0x58, cli, impl);
        INSTR_CASE(0x59, eor, absy);
        INSTR_CASE(0x5D, eor, absx);
        INSTR_CASE(0x5E, lsr, absx);
        INSTR_CASE(0x60, rts, impl);
        INSTR_CASE(0x61, adc, indx);
        INSTR_CASE(0x65, adc, zero);
        INSTR_CASE(0x66, ror, zero);
        INSTR_CASE(0x68, pla, impl);
        INSTR_CASE(0x69, adc, imm);
        INSTR_CASE(0x6A, ror, accum);
        INSTR_CASE(0x6C, jmp, ind);
        INSTR_CASE(0x6D, adc, abs);
        INSTR_CASE(0x6E, ror, abs);
        INSTR_CASE(0x70, bvs, impl);
        INSTR_CASE(0x71, adc, indy);
        INSTR_CASE(0x75, adc, zerox);
        INSTR_CASE(0x76, ror, zerox);
        INSTR_CASE(0x78, sei, impl);
        INSTR_CASE(0x79, adc, absy);
        INSTR_CASE(0x7D, adc, absx);
        INSTR_CASE(0x7E, ror, absx);
        INSTR_CASE(0x81, sta, indx);
        INSTR_CASE(0x84, sty, zero);
        INSTR_CASE(0x85, sta, zero);
        INSTR_CASE(0x86, stx, zero);
        INSTR_CASE(0x88, dey, impl);
        INSTR_CASE(0x8A, txa, impl);
        INSTR_CASE(0x8C, sty, abs);
        INSTR_CASE(0x8D, sta, abs);
        INSTR_CASE(0x8E, stx, abs);
        INSTR_CASE(0x90, bcc, impl);
        INSTR_CASE(0x91, sta, indy);
        INSTR_CASE(0x94, sty, zerox);
        INSTR_CASE(0x95, sta, zerox);
        INSTR_CASE(0x96, stx, zeroy);
        INSTR_CASE(0x98, tya, impl);
        INSTR_CASE(0x99, sta, absy);
        INSTR_CASE(0x9A, txs, impl);
        INSTR_CASE(0x9D, sta, absx);
        INSTR_CASE(0xA0, ldy, imm);
        INSTR_CASE(0xA1, lda, indx);
        INSTR_CASE(0xA2, ldx, imm);
        INSTR_CASE(0xA4, ldy, zero);
        INSTR_CASE(0xA5, lda, zero);
        INSTR_CASE(0xA6, ldx, zero);
        INSTR_CASE(0xA8, tay, impl);
        INSTR_CASE(0xA9, lda, imm);
        INSTR_CASE(0xAA, tax, impl);
        INSTR_CASE(0xAC, ldy, abs);
        INSTR_CASE(0xAD, lda, abs);
        INSTR_CASE(0xAE, ldx, abs);
        INSTR_CASE(0xB0, bcs, impl);
        INSTR_CASE(0xB1, lda, indy);
        INSTR_CASE(0xB4, ldy, zerox);
        INSTR_CASE(0xB5, lda, zerox);
        INSTR_CASE(0xB6, ldx, zeroy);
        INSTR_CASE(0xB8, clv, impl);
        INSTR_CASE(0xB9, lda, absy);
        INSTR_CASE(0xBA, tsx, impl);
        INSTR_CASE(0xBC, ldy, absx);
        INSTR_CASE(0xBD, lda, absx);
        INSTR_CASE(0xBE, ldx, absy);
        INSTR_CASE(0xC0, cpy, imm);
        INSTR_CASE(0xC1, cmp, indx);
        INSTR_CASE(0xC4, cpy, zero);
        INSTR_CASE(0xC5, cmp, zero);
        INSTR_CASE(0xC6, dec, zero);
        INSTR_CASE(0xC8, iny, impl);
        INSTR_CASE(0xC9, cmp, imm);
        INSTR_CASE(0xCA, dex, impl);
        INSTR_CASE(0xCC, cpy, abs);
        INSTR_CASE(0xCD, cmp, abs);
        INSTR_CASE(0xCE, dec, abs);
        INSTR_CASE(0xD0, bne, impl);
        INSTR_CASE(0xD1, cmp, indy);
        INSTR_CASE(0xD5, cmp, zerox);
        INSTR_CASE(0xD6, dec, zerox);
        INSTR_CASE(0xD9, cmp, absy);
        INSTR_CASE(0xDD, cmp, absx);
        INSTR_CASE(0xDE, dec, absx);
        INSTR_CASE(0xE0, cpx, imm);
        INSTR_CASE(0xE1, sbc, indx);
        INSTR_CASE(0xE4, cpx, zero);
        INSTR_CASE(0xE5, sbc, zero);
        INSTR_CASE(0xE6, inc, zero);
        INSTR_CASE(0xE8, inx, impl);
        INSTR_CASE(0xE9, sbc, imm);
        INSTR_CASE(0xEA, nop, impl);
        INSTR_CASE(0xEC, cpx, abs);
        INSTR_CASE(0xED, sbc, abs);
        INSTR_CASE(0xEE, inc, abs);
        INSTR_CASE(0xF0, beq, impl);
        INSTR_CASE(0xF1, sbc, indy);
        INSTR_CASE(0xF5, sbc, zerox);
        INSTR_CASE(0xF6, inc, zerox);
        INSTR_CASE(0xF9, sbc, absy);
        INSTR_CASE(0xFD, sbc, absx);
        INSTR_CASE(0xFE, inc, absx);
    }
}

#undef INSTR_CASE

uint8_t CPU::fetch_op()
{
    return 0;
}

void CPU::cmp(uint8_t reg, uint8_t val)
{
    uint8_t tmp = reg - val;
    if (tmp == 0)
        procstatus.zero = 1;
    else if ((tmp & 0x80) == 1)
        procstatus.neg = 1;
    //else carry
}

void CPU::jmp(uint16_t addr)
{
}

void CPU::push(uint8_t val)
{

}

uint8_t CPU::pull()
{
    return 0;
}
