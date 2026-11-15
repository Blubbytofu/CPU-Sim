
#ifndef ISS_CORE_HPP
#define ISS_CORE_HPP

#include <array>
#include <cstdint>
#include <format>
#include <optional>
#include <string>
#include <vector>

#include "instruction.hpp"
#include "memory.hpp"
#include "utils/bit_utils.hpp"

struct ArchitecturalState {
    uint32_t pc{0};
    std::array<uint32_t, 32> reg{};

    ArchitecturalState(void) = default;
};

inline std::string to_string(const ArchitecturalState& state) {
    std::string str = std::format("PC = 0x{:08X}\n", state.pc);

    for (std::size_t i = 0; i < state.reg.size(); i++) {
        str += std::format("x{:<2} = 0x{:08X}", i, state.reg[i]);
        str += ((i + 1) % 4 == 0) ? "\n" : "  ";
    }

    return str;
}

struct RegisterWrite {
    uint8_t rd{0};
    uint32_t value{0};

    RegisterWrite(void) = default;
    RegisterWrite(uint8_t d, uint32_t val) : rd{d}, value{val} {}
};

struct MemoryWrite {
    uint32_t addr{0};
    uint32_t value{0};
    Alignment alignment{Alignment::WORD};

    MemoryWrite(void) = default;
    MemoryWrite(uint32_t ad, uint32_t v, Alignment al) : addr{ad}, value{v}, alignment{al} {}
};

enum class TrapType {
    INSTRUCTION_ADDRESS_MISALIGNED = 0,
    INSTRUCTION_ACCESS_FAULT = 1,
    ILLEGAL_INSTRUCTION = 2,
    BREAKPOINT = 3,
    LOAD_ACCESS_MISALIGNED = 4,
    LOAD_ACCESS_FAULT = 5,
    STORE_ACCESS_MISALIGNED = 6,
    STORE_ACCESS_FAULT = 7,
    ENVIRONMENT_CALL_MMODE = 11
};

inline std::string to_string(TrapType type) {
    switch (type) {
    case TrapType::INSTRUCTION_ADDRESS_MISALIGNED:
        return "instruction-address-misaligned";
    case TrapType::INSTRUCTION_ACCESS_FAULT:
        return "instruction-access-fault";
    case TrapType::ILLEGAL_INSTRUCTION:
        return "illegal-instruction";
    case TrapType::BREAKPOINT:
        return "breakpoint";
    case TrapType::LOAD_ACCESS_MISALIGNED:
        return "load-address-misaligned";
    case TrapType::LOAD_ACCESS_FAULT:
        return "load-access-fault";
    case TrapType::STORE_ACCESS_MISALIGNED:
        return "store-address-misaligned";
    case TrapType::STORE_ACCESS_FAULT:
        return "store-access-fault";
    case TrapType::ENVIRONMENT_CALL_MMODE:
        return "environment-call-mmode";
    default:
        return "unknown-trap";
    }
}

struct Trap {
    TrapType type{TrapType::INSTRUCTION_ADDRESS_MISALIGNED};
    uint32_t value{0};

    Trap(void) = default;
    Trap(TrapType t, uint32_t c) : type{t}, value{c} {}
};

struct ExecutionStep {
    uint32_t pc{0};
    uint32_t next_pc{0};

    std::optional<Instruction> inst{};
    std::optional<RegisterWrite> reg_write{std::nullopt};
    std::optional<MemoryWrite> mem_write{std::nullopt};
    std::optional<Trap> trap{std::nullopt};

    ExecutionStep() = default;
};

inline std::string to_string(const ExecutionStep& step) {
    std::string str = std::format("PC={:08X} ", step.pc);
    if (step.inst) {
        str += std::format("{:08X}  {}", step.inst->raw, to_string(*step.inst));
    }
    else {
        str += "<instruction not fetched>";
    }

    if (step.trap) {
        str += std::format(" | TRAP={} value=0x{:08X} | next_pc=0x{:08X}", to_string(step.trap->type), step.trap->value, step.next_pc);
        return str + "\n";
    }

    if (step.reg_write) {
        str += std::format(" | x{}=0x{:08X}", step.reg_write->rd, step.reg_write->value);
    }

    if (step.mem_write) {
        uint32_t num_bytes = static_cast<uint32_t>(step.mem_write->alignment);
        uint32_t write_val= step.mem_write->value;
        if (num_bytes == 1) {
            write_val &= 0xFFU;
        }
        if (num_bytes == 2) {
            write_val &= 0xFFFFU;
        }

        str += std::format(" | mem[0x{:08X}, {} bytes]=0x{:08X}", step.mem_write->addr, num_bytes, write_val);
    }

    str += std::format(" | next_pc=0x{:08X}", step.next_pc);

    return str + "\n";
}

class ISSCPU {
private:
    ArchitecturalState state;
    Memory mem;
    Decoder decoder;

public:
    ISSCPU(void) = default;
    ~ISSCPU(void) = default;

    std::string expose_state() const;
    void reset(uint32_t entry_pc);
    void load_program(uint32_t addr, const std::vector<uint32_t>& data);
    ExecutionStep step(void);
};

#endif
