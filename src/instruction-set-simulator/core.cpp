
#include "core.hpp"

#include <cassert>
#include <exception>

std::string ISSCPU::expose_state() const {
    return to_string(state);
}

void ISSCPU::reset(uint32_t entry_pc) {
    if (!is_aligned(entry_pc, Alignment::WORD)) {
        assert(false && "Misaligned PC.");
    }
    state.pc = entry_pc;
}

void ISSCPU::load_program(uint32_t addr, const std::vector<uint32_t>& data) {
    if (!is_aligned(addr, Alignment::WORD)) {
        assert(false && "Misaligned Program.");
    }

    for (int i = 0; i < data.size(); i++) {
        if (!mem.can_access(addr, Alignment::WORD, MemoryAccessType::STORE)) {
            assert(false && "Cannot Write Program into Region.");
        }
        mem.write_32(addr + 4 * i, data[i]);
    }
}

ExecutionStep ISSCPU::step(void) {
    ExecutionStep step{};
    step.pc = state.pc;
    step.next_pc = state.pc;

    if (!mem.can_access(state.pc, Alignment::WORD, MemoryAccessType::INST_FETCH)) {
        step.trap = Trap{TrapType::INSTRUCTION_ACCESS_FAULT, state.pc};
        return step;
    }

    std::optional<RegisterWrite> pending_reg_write = std::nullopt;
    std::optional<MemoryWrite> pending_mem_write = std::nullopt;
    uint32_t next_pc = state.pc + 4;

    uint32_t inst_raw = mem.read_32(state.pc);
    step.inst = decoder.decode_instruction(inst_raw);

    uint8_t rd = step.inst->rd;
    uint32_t rs1_val = state.reg[step.inst->rs1];
    uint32_t rs2_val = state.reg[step.inst->rs2];
    uint32_t imm = step.inst->imm;

    switch (step.inst->name) {
    case InstructionName::LUI:
        pending_reg_write = RegisterWrite{rd, imm};
        break;

	case InstructionName::AUIPC:
        pending_reg_write = RegisterWrite{rd, state.pc + imm};
        break;

	case InstructionName::JAL:
        pending_reg_write = RegisterWrite{rd, state.pc + 4};
        next_pc = state.pc + imm;
        break;

	case InstructionName::JALR:
        pending_reg_write = RegisterWrite{rd, state.pc + 4};
        next_pc = (rs1_val + imm) & 0xFFFFFFFE;
        break;

	case InstructionName::BEQ:
        if (rs1_val == rs2_val) {
            next_pc = state.pc + imm;
        }
        break;

	case InstructionName::BNE:
        if (rs1_val != rs2_val) {
            next_pc = state.pc + imm;
        }
        break;

	case InstructionName::BLT:
        if (static_cast<int32_t>(rs1_val) < static_cast<int32_t>(rs2_val)) {
            next_pc = state.pc + imm;
        }
        break;

	case InstructionName::BGE:
        if (static_cast<int32_t>(rs1_val) >= static_cast<int32_t>(rs2_val)) {
            next_pc = state.pc + imm;
        }
        break;

	case InstructionName::BLTU:
        if (rs1_val < rs2_val) {
            next_pc = state.pc + imm;
        }
        break;

	case InstructionName::BGEU:
        if (rs1_val >= rs2_val) {
            next_pc = state.pc + imm;
        }
        break;

	case InstructionName::LB:
        if (!mem.can_access(rs1_val + imm, Alignment::BYTE, MemoryAccessType::LOAD)) {
            step.trap = Trap{TrapType::LOAD_ACCESS_FAULT, rs1_val + imm};
            return step;
        }
        pending_reg_write = RegisterWrite{rd, sign_extend(mem.read_8(rs1_val + imm), 8)};
        break;

	case InstructionName::LH:
        if (!mem.can_access(rs1_val + imm, Alignment::HALF_WORD, MemoryAccessType::LOAD)) {
            step.trap = Trap{TrapType::LOAD_ACCESS_FAULT, rs1_val + imm};
            return step;
        }
        if (!is_aligned(rs1_val + imm, Alignment::HALF_WORD)) {
            step.trap = Trap{TrapType::LOAD_ACCESS_MISALIGNED, rs1_val + imm};
            return step;
        }
        pending_reg_write = RegisterWrite{rd, sign_extend(mem.read_16(rs1_val + imm), 16)};
        break;

	case InstructionName::LW:
        if (!mem.can_access(rs1_val + imm, Alignment::WORD, MemoryAccessType::LOAD)) {
            step.trap = Trap{TrapType::LOAD_ACCESS_FAULT, rs1_val + imm};
            return step;
        }
        if (!is_aligned(rs1_val + imm, Alignment::WORD)) {
            step.trap = Trap{TrapType::LOAD_ACCESS_MISALIGNED, rs1_val + imm};
            return step;
        }
        pending_reg_write = RegisterWrite{rd, mem.read_32(rs1_val + imm)};
        break;

	case InstructionName::LBU:
        if (!mem.can_access(rs1_val + imm, Alignment::BYTE, MemoryAccessType::LOAD)) {
            step.trap = Trap{TrapType::LOAD_ACCESS_FAULT, rs1_val + imm};
            return step;
        }
        pending_reg_write = RegisterWrite{rd, mem.read_8(rs1_val + imm)};
        break;

	case InstructionName::LHU:
        if (!mem.can_access(rs1_val + imm, Alignment::HALF_WORD, MemoryAccessType::LOAD)) {
            step.trap = Trap{TrapType::LOAD_ACCESS_FAULT, rs1_val + imm};
            return step;
        }
        if (!is_aligned(rs1_val + imm, Alignment::HALF_WORD)) {
            step.trap = Trap{TrapType::LOAD_ACCESS_MISALIGNED, rs1_val + imm};
            return step;
        }
        pending_reg_write = RegisterWrite{rd, mem.read_16(rs1_val + imm)};
        break;

	case InstructionName::SB:
        if (!mem.can_access(rs1_val + imm, Alignment::BYTE, MemoryAccessType::STORE)) {
            step.trap = Trap{TrapType::STORE_ACCESS_FAULT, rs1_val + imm};
            return step;
        }
        pending_mem_write = MemoryWrite{rs1_val + imm, rs2_val, Alignment::BYTE};
        break;

	case InstructionName::SH:
        if (!mem.can_access(rs1_val + imm, Alignment::HALF_WORD, MemoryAccessType::STORE)) {
            step.trap = Trap{TrapType::STORE_ACCESS_FAULT, rs1_val + imm};
            return step;
        }
        if (!is_aligned(rs1_val + imm, Alignment::HALF_WORD)) {
            step.trap = Trap{TrapType::STORE_ACCESS_MISALIGNED, rs1_val + imm};
            return step;
        }
        pending_mem_write = MemoryWrite{rs1_val + imm, rs2_val, Alignment::HALF_WORD};
        break;

	case InstructionName::SW:
        if (!mem.can_access(rs1_val + imm, Alignment::WORD, MemoryAccessType::STORE)) {
            step.trap = Trap{TrapType::STORE_ACCESS_FAULT, rs1_val + imm};
            return step;
        }
        if (!is_aligned(rs1_val + imm, Alignment::WORD)) {
            step.trap = Trap{TrapType::STORE_ACCESS_MISALIGNED, rs1_val + imm};
            return step;
        }
        pending_mem_write = MemoryWrite{rs1_val + imm, rs2_val, Alignment::WORD};
        break;

	case InstructionName::ADDI:
        pending_reg_write = RegisterWrite{rd, rs1_val + imm};
        break;

	case InstructionName::SLTI:
        if (static_cast<int32_t>(rs1_val) < static_cast<int32_t>(imm)) {
            pending_reg_write = RegisterWrite{rd, 1};
        }
        else {
            pending_reg_write = RegisterWrite{rd, 0};
        }
        break;

	case InstructionName::SLTIU:
        if (rs1_val < imm) {
            pending_reg_write = RegisterWrite{rd, 1};
        }
        else {
            pending_reg_write = RegisterWrite{rd, 0};
        }
        break;

	case InstructionName::XORI:
        pending_reg_write = RegisterWrite{rd, rs1_val ^ imm};
        break;

	case InstructionName::ORI:
        pending_reg_write = RegisterWrite{rd, rs1_val | imm};
        break;

	case InstructionName::ANDI:
        pending_reg_write = RegisterWrite{rd, rs1_val & imm};
        break;

	case InstructionName::SLLI:
        pending_reg_write = RegisterWrite{rd, rs1_val << imm};
        break;

	case InstructionName::SRLI:
        pending_reg_write = RegisterWrite{rd, rs1_val >> imm};
        break;

	case InstructionName::SRAI:
        pending_reg_write = RegisterWrite{rd, static_cast<uint32_t>(static_cast<int32_t>(rs1_val) >> (imm & 0b11111))};
        break;

	case InstructionName::ADD:
        pending_reg_write = RegisterWrite{rd, rs1_val + rs2_val};
        break;

	case InstructionName::SUB:
        pending_reg_write = RegisterWrite{rd, rs1_val - rs2_val};
        break;

	case InstructionName::SLL:
        pending_reg_write = RegisterWrite{rd, rs1_val << (rs2_val & 0b11111)};
        break;

	case InstructionName::SLT:
        if (static_cast<int32_t>(rs1_val) < static_cast<int32_t>(rs2_val)) {
            pending_reg_write = RegisterWrite{rd, 1};
        }
        else {
            pending_reg_write = RegisterWrite{rd, 0};
        }
        break;

	case InstructionName::SLTU:
        if (rs1_val < rs2_val) {
            pending_reg_write = RegisterWrite{rd, 1};
        }
        else {
            pending_reg_write = RegisterWrite{rd, 0};
        }
        break;

	case InstructionName::XOR:
        pending_reg_write = RegisterWrite{rd, rs1_val ^ rs2_val};
        break;

	case InstructionName::SRL:
        pending_reg_write = RegisterWrite{rd, rs1_val >> (rs2_val & 0b11111)};
        break;

	case InstructionName::SRA:
        pending_reg_write = RegisterWrite{rd, static_cast<uint32_t>(static_cast<int32_t>(rs1_val) >> (rs2_val & 0b11111))};
        break;

	case InstructionName::OR:
        pending_reg_write = RegisterWrite{rd, rs1_val | rs2_val};
        break;

	case InstructionName::AND:
        pending_reg_write = RegisterWrite{rd, rs1_val & rs2_val};
        break;

	case InstructionName::FENCE:
	case InstructionName::FENCE_TSO:
	case InstructionName::PAUSE:
        // nop for now
        break;

	case InstructionName::ECALL:
        step.trap = Trap{TrapType::ENVIRONMENT_CALL_MMODE, step.inst->raw};
        return step;

	case InstructionName::EBREAK:
        step.trap = Trap{TrapType::BREAKPOINT, step.inst->raw};
        return step;

	case InstructionName::INVALID:
        step.trap = Trap{TrapType::ILLEGAL_INSTRUCTION, step.inst->raw};
        return step;

	default:
        assert(false && "Unknown Instruction Snuck Past the Decoder.");
    }

    if (!is_aligned(next_pc, Alignment::WORD)) {
        step.trap = Trap{TrapType::INSTRUCTION_ADDRESS_MISALIGNED, next_pc};
        return step;
    }

    if (pending_reg_write && pending_reg_write->rd != 0) {
        step.reg_write = pending_reg_write;

        state.reg[pending_reg_write->rd] = pending_reg_write->value;
    }

    if (pending_mem_write) {
        step.mem_write = pending_mem_write;

        switch (pending_mem_write->alignment) {
        case Alignment::BYTE:
            mem.write_8(pending_mem_write->addr, static_cast<uint8_t>(pending_mem_write->value));
            break;

        case Alignment::HALF_WORD:
            mem.write_16(pending_mem_write->addr, static_cast<uint16_t>(pending_mem_write->value));
            break;

        case Alignment::WORD:
            mem.write_32(pending_mem_write->addr, pending_mem_write->value);
            break;

        case Alignment::DOUBLE_WORD:
            assert(false && "Memory Write Width D-WORD Not Implemented Yet.");

        default:
            assert(false && "Invalid Memory Write Width.");
        }
    }

    step.next_pc = next_pc;

    state.pc = next_pc;
    state.reg[0] = 0;

    return step;
}
