
#ifndef ISS_MEMORY_HPP
#define ISS_MEMORY_HPP

#include <array>
#include <cstdint>
#include <unordered_map>

#include "utils/bit_utils.hpp"

constexpr uint64_t PAGE_BITS = 12;
constexpr uint64_t PAGE_SIZE = 1ULL << PAGE_BITS;
constexpr uint64_t PAGE_MASK = PAGE_SIZE - 1;

enum class Endianness {
    LITTLE,
    BIG
};

enum class MemoryAccessType {
    INST_FETCH,
    LOAD,
    STORE
};

struct Page {
    std::array<uint8_t, PAGE_SIZE> bytes{};

    Page(void) = default;
};

class Memory {
private:
    std::unordered_map<uint64_t, Page> page_table;

public:
    Memory() = default;
    ~Memory() = default;

    uint8_t read_8(uint64_t addr) const;
    uint16_t read_16(uint64_t addr) const;
    uint32_t read_32(uint64_t addr) const;
    // uint64_t read_64(uint64_t addr) const;

    void write_8(uint64_t addr, uint8_t value);
    void write_16(uint64_t addr, uint16_t value);
    void write_32(uint64_t addr, uint32_t value);
    // void write_64(uint64_t addr, uint64_t value);

    bool can_access(uint32_t addr, Alignment size, MemoryAccessType type);
};

#endif
