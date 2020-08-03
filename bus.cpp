#include "bus.h"

#include <cstdio>
#include <cstring>

/* copies rom memory and initizializes all other memory to 0 */
void Bus::initmem(uint8_t *prgrom)
{
    std::memset(memory, 0, PRGROM_START-1);
    std::memcpy(memory+PRGROM_START, prgrom, PRGROM_SIZE);
}

/* reads memory from the specified address */
uint8_t Bus::read(uint16_t addr)
{
    return memory[addr];
}

/* writes val to the specified address */
void Bus::write(uint16_t addr, uint8_t val)
{
    if (!write_enable)
        return;
    memory[addr] = val;
}

/* prints the contents of current memory to a file with name fname */
void Bus::memdump(const char * const fname)
{
    int i, j;
    FILE *f;

    f = fopen(fname, "w");
    for (i = 0; i < MEMSIZE; ) {
        std::fprintf(f, "%04X: ", i);
        for (j = 0; j < 16; j++) {
            std::fprintf(f, "%02X ", memory[i]);
            i++;
        }
        std::fputs("\n", f);
    }
    fclose(f);
}

