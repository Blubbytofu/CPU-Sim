
#include "memory.hpp"

uint8_t Memory::read_8(uint64_t addr) const {
    uint64_t page_num = addr >> PAGE_BITS;
    uint64_t offset = addr & PAGE_MASK;
    if (!page_table.count(page_num)) {
        return 0;
    }
    return page_table.at(page_num).bytes[offset];
}

uint16_t Memory::read_16(uint64_t addr) const {
    uint16_t result = 0;
    for (int i = 0; i < 2; i++) {
        result |= static_cast<uint16_t>(
            static_cast<uint32_t>(read_8(addr + i)) << (8 * i)
        );
    }
    return result;
}

uint32_t Memory::read_32(uint64_t addr) const {
    uint32_t result = 0;
    for (int i = 0; i < 4; i++) {
        result |= static_cast<uint32_t>(read_8(addr + i)) << (8 * i);
    }
    return result;
}

void Memory::write_8(uint64_t addr, uint8_t value) {
    uint64_t page_num = addr >> PAGE_BITS;
    uint64_t offset = addr & PAGE_MASK;
    page_table[page_num].bytes[offset] = value;
}

void Memory::write_16(uint64_t addr, uint16_t value) {
    for (int i = 0; i < 2; i++) {
        uint8_t byte = static_cast<uint8_t>(value & 0xFF);
        write_8(addr + i, byte);
        value >>= 8;
    }
}

void Memory::write_32(uint64_t addr, uint32_t value) {
    for (int i = 0; i < 4; i++) {
        uint8_t byte = static_cast<uint8_t>(value & 0xFF);
        write_8(addr + i, byte);
        value >>= 8;
    }
}

bool Memory::can_access(uint32_t addr, Alignment size, MemoryAccessType type) {
    addr = 0;
    size = Alignment::BYTE;
    type = MemoryAccessType::INST_FETCH;
    return true;
}
