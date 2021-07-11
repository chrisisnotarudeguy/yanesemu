#pragma once

#include <emu/core/const.hpp>
#include <emu/util/array.hpp>
#include <emu/util/unsigned.hpp>

class Screen {
    struct ColorRGBA {
        uint8 data[4];

        ColorRGBA() = default;
        ColorRGBA(uint32 value)
        {
            data[0] = value >> 24 & 0xFF;
            data[1] = value >> 16 & 0xFF;
            data[2] = value >> 8  & 0xFF;
            data[3] = value       & 0xFF;
        }
    };

    Util::Array2D<ColorRGBA, Core::SCREEN_WIDTH, Core::SCREEN_HEIGHT> buf;
    uint32 *pal;

public:
    enum class Palette {
        PAL2C02, PAL2C03, RC2C03B,
    };

    Screen();

    void output(unsigned x, unsigned y, uint6 value);
    void set_palette(Palette palette);

    uint32 *data() { return (uint32 *) buf.data(); }
};