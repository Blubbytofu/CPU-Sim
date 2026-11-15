
#include "instruction.hpp"

#include "utils/bit_utils.hpp"

// |31           funct7                     25|24        rs2        20|19    rs1    15|14    funct3    12|11             rd            7|6    opcode    0|
void Decoder::parse_R(uint32_t raw, Instruction& inst) {
    // inst.raw

    // inst.type = InstructionType::R;
    // inst.name;

    inst.funct3 = extract_funct3(raw);
    inst.funct7 = extract_funct7(raw);
    // inst.opcode

    inst.rd = extract_rd(raw);
    inst.rs1 = extract_rs1(raw);
    inst.rs2 = extract_rs2(raw);

    // inst.imm;
}

// |31                     imm[11:0]                                20|19    rs1    15|14    funct3    12|11             rd            7|6    opcode    0|
void Decoder::parse_I(uint32_t raw, Instruction& inst) {
    // inst.raw

    // inst.type
    // inst.name;

    inst.funct3 = extract_funct3(raw);
    // inst.funct7
    // inst.opcode

    inst.rd = extract_rd(raw);
    inst.rs1 = extract_rs1(raw);
    // inst.rs2

    // we need to do different things for SLLI, SRLI, SRAI
    uint32_t imm = extract_bits(raw, 20, 12);
    inst.imm = sign_extend(imm, 12);
}

// |31                     imm[11:5]        25|24        rs2        20|19    rs1    15|14    funct3    12|11    imm[4:0]               7|6    opcode    0|
void Decoder::parse_S(uint32_t raw, Instruction& inst) {
    // inst.raw

    // inst.type
    // inst.name;

    inst.funct3 = extract_funct3(raw);
    // inst.funct7
    // inst.opcode

    // inst.rd
    inst.rs1 = extract_rs1(raw);
    inst.rs2 = extract_rs2(raw);

    uint32_t imm =
        (extract_bits(raw, 25, 7) << 5) |
        extract_bits(raw, 7, 5);
    inst.imm = sign_extend(imm, 12);
}

// |31  [12]  31|30        imm[10:5]        25|24        rs2        20|19    rs1    15|14    funct3    12|11    imm[4:1]    8|7  [11]  7|6    opcode    0|
void Decoder::parse_B(uint32_t raw, Instruction& inst) {
    // inst.raw

    // inst.type
    // inst.name;

    inst.funct3 = extract_funct3(raw);
    // inst.funct7
    // inst.opcode

    // inst.rd
    inst.rs1 = extract_rs1(raw);
    inst.rs2 = extract_rs2(raw);

    // bit imm[0] is 0
    uint32_t imm = 
        (extract_bits(raw, 31, 1) << 12) |
        (extract_bits(raw, 7, 1) << 11) |
        (extract_bits(raw, 25, 6) << 5) |
        (extract_bits(raw, 8, 4) << 1);
    inst.imm = sign_extend(imm, 13);
}

// |31                     imm[31:12]                                                                  12|11             rd            7|6    opcode    0|
void Decoder::parse_U(uint32_t raw, Instruction& inst) {
    // inst.raw

    // inst.type
    // inst.name;

    // inst.funct3
    // inst.funct7
    // inst.opcode

    inst.rd = extract_rd(raw);
    // inst.rs1
    // inst.rs2

    inst.imm = extract_bits(raw, 12, 20) << 12;
}

// |31  [20]  31|30        imm[10:1]                   21|20  [11]  20|19          imm[19:12]          12|11             rd            7|6    opcode    0|
void Decoder::parse_J(uint32_t raw, Instruction& inst) {
    // inst.raw

    // inst.type
    // inst.name;

    // inst.funct3
    // inst.funct7
    // inst.opcode

    inst.rd = extract_rd(raw);
    // inst.rs1
    // inst.rs2

    // bit imm[0] is 0
    uint32_t imm =
        (extract_bits(raw, 31, 1) << 20) |
        (extract_bits(raw, 12, 8) << 12) |
        (extract_bits(raw, 20, 1) << 11) |
        (extract_bits(raw, 21, 10) << 1);
    inst.imm = sign_extend(imm, 21);
}

// |31                                      25|24                   20|19           15|14    funct3    12|11                           7|6              0|
uint8_t Decoder::extract_funct3(uint32_t raw) {
    return static_cast<uint8_t>(extract_bits(raw, 12, 3));
}

// |31           funct7                     25|24                   20|19           15|14              12|11                           7|6              0|
uint8_t Decoder::extract_funct7(uint32_t raw) {
    return static_cast<uint8_t>(extract_bits(raw, 25, 7));
}

// |31                                      25|24                   20|19           15|14              12|11                           7|6    opcode    0|
uint8_t Decoder::extract_opcode(uint32_t raw) {
    return static_cast<uint8_t>(extract_bits(raw, 0, 7));
}

// |31                                      25|24                   20|19           15|14              12|11             rd            7|6              0|
uint8_t Decoder::extract_rd(uint32_t raw) {
    return static_cast<uint8_t>(extract_bits(raw, 7, 5));
}

// |31                                      25|24                   20|19    rs1    15|14              12|11                           7|6              0|
uint8_t Decoder::extract_rs1(uint32_t raw) {
    return static_cast<uint8_t>(extract_bits(raw, 15, 5));
}

// |31                                      25|24        rs2        20|19           15|14              12|11                           7|6              0|
uint8_t Decoder::extract_rs2(uint32_t raw) {
    return static_cast<uint8_t>(extract_bits(raw, 20, 5));
}

void Decoder::validate_R_arithmetic(Instruction& inst) {
    switch (inst.funct3) {
    case 0b000:
        if (inst.funct7 == 0b0000000) {
            inst.name = InstructionName::ADD;
        }
        else if (inst.funct7 == 0b0100000) {
            inst.name = InstructionName::SUB;
        }
        else {
            inst.name = InstructionName::INVALID;
        }
        break;

    case 0b001:
        if (inst.funct7 == 0b0000000) {
            inst.name = InstructionName::SLL;
        }
        else {
            inst.name = InstructionName::INVALID;
        }
        break;

    case 0b010:
        if (inst.funct7 == 0b0000000) {
            inst.name = InstructionName::SLT;
        }
        else {
            inst.name = InstructionName::INVALID;
        }
        break;

    case 0b011:
        if (inst.funct7 == 0b0000000) {
            inst.name = InstructionName::SLTU;
        }
        else {
            inst.name = InstructionName::INVALID;
        }
        break;

    case 0b100:
        if (inst.funct7 == 0b0000000) {
            inst.name = InstructionName::XOR;
        }
        else {
            inst.name = InstructionName::INVALID;
        }
        break;

    case 0b101:
        if (inst.funct7 == 0b0000000) {
            inst.name = InstructionName::SRL;
        }
        else if (inst.funct7 == 0b0100000) {
            inst.name = InstructionName::SRA;
        }
        else {
            inst.name = InstructionName::INVALID;
        }
        break;

    case 0b110:
        if (inst.funct7 == 0b0000000) {
            inst.name = InstructionName::OR;
        }
        else {
            inst.name = InstructionName::INVALID;
        }
        break;

    case 0b111:
        if (inst.funct7 == 0b0000000) {
            inst.name = InstructionName::AND;
        }
        else {
            inst.name = InstructionName::INVALID;
        }
        break;

    default:
        inst.name = InstructionName::INVALID;
        break;
    }
}

void Decoder::validate_load(Instruction& inst) {
    switch (inst.funct3) {
    case 0b000:
        inst.name = InstructionName::LB;
        break;

    case 0b001:
        inst.name = InstructionName::LH;
        break;

    case 0b010:
        inst.name = InstructionName::LW;
        break;

    case 0b100:
        inst.name = InstructionName::LBU;
        break;

    case 0b101:
        inst.name = InstructionName::LHU;
        break;

    default:
        inst.name = InstructionName::INVALID;
    }
}

void Decoder::validate_I_arithmetic(Instruction& inst) {
    // for 0b001 and 0b101 only
    uint32_t shamt_upper = extract_bits(inst.imm, 5, 7);
    uint32_t shamt_lower = extract_bits(inst.imm, 0, 5);
    
    switch (inst.funct3) {
    case 0b000:
        inst.name = InstructionName::ADDI;
        break;

    case 0b010:
        inst.name = InstructionName::SLTI;
        break;

    case 0b011:
        inst.name = InstructionName::SLTIU;
        break;

    case 0b100:
        inst.name = InstructionName::XORI;
        break;

    case 0b110:
        inst.name = InstructionName::ORI;
        break;

    case 0b111:
        inst.name = InstructionName::ANDI;
        break;

    case 0b001:
        if (shamt_upper == 0b0000000) {
            inst.name = InstructionName::SLLI;
            inst.imm = shamt_lower;
        }
        else {
            inst.name = InstructionName::INVALID;
        }
        break;

    case 0b101:
        if (shamt_upper == 0b0000000) {
            inst.name = InstructionName::SRLI;
            inst.imm = shamt_lower;
        }
        else if (shamt_upper == 0b0100000) {
            inst.name = InstructionName::SRAI;
            inst.imm = shamt_lower;
        }
        else {
            inst.name = InstructionName::INVALID;
        }
        break;

    default:
        inst.name = InstructionName::INVALID;
        break;
    }
}

void Decoder::validate_misc_mem(Instruction& inst) {
    if (inst.funct3 == 0b000) {
        inst.name = InstructionName::FENCE;
        uint32_t fm = extract_bits(inst.imm, 8, 4);
        uint32_t pred = extract_bits(inst.imm, 4, 4);
        uint32_t succ = extract_bits(inst.imm, 0, 4);
        if (inst.rs1 == 0b00000 && inst.rd == 0b00000) {
            if (fm == 0b0000 && pred == 0b0001 && succ == 0b0000) {
                inst.name = InstructionName::PAUSE;
            }
            else if (fm == 0b1000 && pred == 0b0011 && succ == 0b0011) {
                inst.name = InstructionName::FENCE_TSO;
            }
        }
    }
    else {
        inst.name = InstructionName::INVALID;
    }
}

void Decoder::validate_system(Instruction& inst) {
    if (inst.funct3 == 0b000) {
        if (inst.rs1 == 0b00000 && inst.rd == 0b00000) {
            if (inst.imm == 0b000000000000) {
                inst.name = InstructionName::ECALL;
            }
            else if (inst.imm == 0b000000000001) {
                inst.name = InstructionName::EBREAK;
            }
            else {
                inst.name = InstructionName::INVALID;
            }
        }
        else {
            inst.name = InstructionName::INVALID;
        }
    }
    else {
        inst.name = InstructionName::INVALID;
    }
}

void Decoder::validate_store(Instruction& inst) {
    switch (inst.funct3) {
    case 0b000:
        inst.name = InstructionName::SB;
        break;

    case 0b001:
        inst.name = InstructionName::SH;
        break;

    case 0b010:
        inst.name = InstructionName::SW;
        break;

    default:
        inst.name = InstructionName::INVALID;
    }
}

void Decoder::validate_branch(Instruction& inst) {
    switch (inst.funct3) {
    case 0b000:
        inst.name = InstructionName::BEQ;
        break;

    case 0b001:
        inst.name = InstructionName::BNE;
        break;

    case 0b100:
        inst.name = InstructionName::BLT;
        break;

    case 0b101:
        inst.name = InstructionName::BGE;
        break;

    case 0b110:
        inst.name = InstructionName::BLTU;
        break;

    case 0b111:
        inst.name = InstructionName::BGEU;
        break;

    default:
        inst.name = InstructionName::INVALID;
    }
}

Instruction Decoder::decode_instruction(uint32_t raw) {
    Instruction inst{};
    inst.raw = raw;
    inst.opcode = extract_opcode(raw);

    // fill fields except for name
    switch (inst.opcode) {
    case 0b0110011:     // 10 R-R arithmetic instructions
        inst.type = InstructionType::R;    
        parse_R(raw, inst);
        break;

    case 0b1100111:     // JALR
    case 0b0000011:     // 5 load instructions
    case 0b0010011:     // 9 R-I arithmetic instructions
    case 0b0001111:     // FENCE(.TSO), PAUSE
    case 0b1110011:     // ECALL, EBREAK
        inst.type = InstructionType::I;
        parse_I(raw, inst);
        break;

    case 0b0100011:     // 3 store instructions
        inst.type = InstructionType::S;
        parse_S(raw, inst);
        break;

    case 0b1100011:     // 6 branch instructions
        inst.type = InstructionType::B;
        parse_B(raw, inst);
        break;

    case 0b0110111:     // LUI
    case 0b0010111:     // AUIPC
        inst.type = InstructionType::U;
        parse_U(raw, inst);
        break;

    case 0b1101111:     // JAL
        inst.type = InstructionType::J;
        parse_J(raw, inst);
        break;
    
    default:
        // inst is default invalid but make sure
        inst.type = InstructionType::INVALID;
        inst.name = InstructionName::INVALID;
        return inst;
    }

    // validate fields for name
    switch (inst.opcode) {
    case 0b0110011:     // 10 R-R arithmetic instructions
        validate_R_arithmetic(inst);
        break;

    case 0b1100111:     // JALR
        if (inst.funct3 == 0b000) {
            inst.name = InstructionName::JALR;
        }
        break;

    case 0b0000011:     // 5 load instructions
        validate_load(inst);
        break;

    case 0b0010011:     // 9 R-I arithmetic instructions
        validate_I_arithmetic(inst);
        break;

    case 0b0001111:     // FENCE(.TSO), PAUSE
        validate_misc_mem(inst);
        break;

    case 0b1110011:     // ECALL, EBREAK
        validate_system(inst);
        break;

    case 0b0100011:     // 3 store instructions
        validate_store(inst);
        break;

    case 0b1100011:     // 6 branch instructions
        validate_branch(inst);
        break;

    case 0b0110111:     // LUI
        inst.name = InstructionName::LUI;
        break;

    case 0b0010111:     // AUIPC
        inst.name = InstructionName::AUIPC;
        break;

    case 0b1101111:     // JAL
        inst.name = InstructionName::JAL;
        break;

    default:
        // should not arrive at this case at all because of the previous default case
        inst.type = InstructionType::INVALID;
        inst.name = InstructionName::INVALID;
        return inst;
    }

    // final check
    if (inst.type == InstructionType::INVALID || inst.name == InstructionName::INVALID) {
        inst.type = InstructionType::INVALID;
        inst.name = InstructionName::INVALID;
    }

    return inst;
}
