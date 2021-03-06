/* Bit operations library.
 * Most routines here are OF THE UTMOST IMPORTANCE. Making an error while
 * changing one means an obscure bug in all the emulator. Proceed with caution.
 */

#ifndef UTIL_BITS_HPP_INCLUDED
#define UTIL_BITS_HPP_INCLUDED

#include <cstdint>

namespace Util {

/* Routines for getting/settings bits at once. More readable, and thus
 * preferable, than using naked complicated bit operations. */
constexpr inline uint64_t getbits(uint64_t num, uint8_t bitno, uint8_t nbits)
{
    return num >> bitno & nbits;
}

constexpr inline uint64_t getbit(uint64_t num, uint8_t bitno)
{
    return getbits(num, bitno, 1);
}

/* Returns a mask usable to mask off a given number of bits.
 * For example: 3 -> 0b11; 6 -> 0b111111 */
constexpr inline uint64_t get_mask_nbits(uint8_t nbits)
{
    return (1UL << nbits) - 1UL;
}

constexpr inline uint64_t setbits(uint64_t num, uint8_t bitno, uint8_t nbits, uint64_t data)
{
    const uint64_t mask = get_mask_nbits(nbits);
    return (num & ~(mask << bitno)) | (data & mask) << bitno;
}

constexpr inline uint64_t setbit(uint64_t num, uint8_t bitno, bool data)
{
    return setbits(num, bitno, 1, data);
}

/* Rotation routines. These do usually convert to a single ROL/ROR instruction. */
constexpr inline uint32_t rotl32(const uint32_t x, const int k)
{
    return (x << k) | (x >> (32 - k));
}

constexpr inline uint32_t rotr32(const uint32_t x, const int k)
{
    return (x >> k) | (x << (32 - k));
}

/* A struct for portable bit-fields. Use it like so:
 * union {
 *     uint16_t full
 *     BitField<1, uint16_t> flag;
 *     BitField<2, uint16_t> flag2;
 *     // ...
 * } data;
 * the types must necessarily be the same or else it won't work at all.
 */
template <typename T, unsigned Bitno, unsigned Nbits = 1>
struct BitField {
    T data;

    // this operator= below is ESSENTIAL. before adding it I got a bunch of bugs
    // when copying BitFields with the same exact types.
    BitField & operator=(const BitField<T, Bitno, Nbits> &oth) { data = setbits(data, Bitno, Nbits, oth.data); return *this; }
    BitField & operator=(const T val)                          { data = setbits(data, Bitno, Nbits, val);      return *this; }
    operator uint64_t() const                                  { return (data >> Bitno) & get_mask_nbits(Nbits); }

    template <typename U> BitField & operator+=(const U val) { *this = *this + val; return *this; }
    template <typename U> BitField & operator-=(const U val) { *this = *this - val; return *this; }
    template <typename U> BitField & operator*=(const U val) { *this = *this * val; return *this; }
    template <typename U> BitField & operator/=(const U val) { *this = *this / val; return *this; }
    template <typename U> BitField & operator&=(const U val) { *this = *this & val; return *this; }
    template <typename U> BitField & operator|=(const U val) { *this = *this | val; return *this; }
    template <typename U> BitField & operator^=(const U val) { *this = *this ^ val; return *this; }
    template <typename U> BitField & operator>>=(const U val) { *this = *this >> val; return *this; }
    template <typename U> BitField & operator<<=(const U val) { *this = *this << val; return *this; }

    BitField & operator++()    { return *this = *this + 1; }
    BitField & operator--()    { return *this = *this - 1; }
};

/* Convert from bytes to a unit. */
inline constexpr unsigned long long to_kib(unsigned long long bytes) { return bytes*1024; }
inline constexpr unsigned long long to_mib(unsigned long long bytes) { return bytes*1024*1024; }
inline constexpr unsigned long long to_gib(unsigned long long bytes) { return bytes*1024*1024*1024; }
inline constexpr unsigned long long to_tib(unsigned long long bytes) { return bytes*1024*1024*1024*1024; }

inline constexpr unsigned long long to_kb(unsigned long long bytes) { return bytes*1000; }
inline constexpr unsigned long long to_mb(unsigned long long bytes) { return bytes*1000*1000; }
inline constexpr unsigned long long to_gb(unsigned long long bytes) { return bytes*1000*1000*1000; }
inline constexpr unsigned long long to_tb(unsigned long long bytes) { return bytes*1000*1000*1000*1000; }

inline constexpr unsigned long long to_kibit(unsigned long long bytes) { return bytes*1024/8; }
inline constexpr unsigned long long to_mibit(unsigned long long bytes) { return bytes*1024*1024/8; }
inline constexpr unsigned long long to_gibit(unsigned long long bytes) { return bytes*1024*1024*1024/8; }
inline constexpr unsigned long long to_tibit(unsigned long long bytes) { return bytes*1024*1024*1024*1024/8; }

} // namespace Util

#endif
