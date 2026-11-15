
#ifndef UTILS_BIT_UTILS_HPP
#define UTILS_BIT_UTILS_HPP

#include <cassert>
#include <cstdint>

// namespace bit_utils {

enum class Alignment {
    BYTE = 1,
    HALF_WORD = 2,
    WORD = 4,
    DOUBLE_WORD = 8
};

constexpr bool is_aligned(uint32_t addr, Alignment alignment) {
    const uint32_t mask = static_cast<uint32_t>(alignment) - 1;
    return (addr & mask) == 0;
}

constexpr uint32_t extract_bits(uint32_t value, uint32_t lsb_pos, uint32_t num_bits) {
    assert(num_bits != 0 && num_bits < 32 && "Invalid of bits extracted.");
    assert(lsb_pos < 32  && "Invalid extract bit window.");
    assert(num_bits <= 32 - lsb_pos && "Invalid extract bit window.");

    const uint32_t mask = (1U << num_bits) - 1U;
    return (value >> lsb_pos) & mask;
}

constexpr uint32_t sign_extend(uint32_t value, uint32_t prev_width) {
    assert(prev_width != 0 && prev_width < 32 && "Invalid previous width.");
    
    // ensure 0s above prev_width
    const uint32_t mask = (1U << prev_width) - 1U;
    value &= mask;

    const uint32_t sign = 1U << (prev_width - 1U);
    return (value ^ sign) - sign;
}

// }

#endif
