
#ifndef ISS_INSTRUCTION_HPP
#define ISS_INSTRUCTION_HPP

#include <cstdint>
#include <format>
#include <string>

enum class InstructionType {
	R,		// |31           funct7                     25|24        rs2        20|19    rs1    15|14    funct3    12|11             rd            7|6    opcode    0|
	I,		// |31                     imm[11:0]                                20|19    rs1    15|14    funct3    12|11             rd            7|6    opcode    0|
	S,		// |31                     imm[11:5]        25|24        rs2        20|19    rs1    15|14    funct3    12|11    imm[4:0]               7|6    opcode    0|
	B,		// |31  [12]  31|30        imm[10:5]        25|24        rs2        20|19    rs1    15|14    funct3    12|11    imm[4:1]    8|7  [11]  7|6    opcode    0|
	U,		// |31                     imm[31:12]                                                                  12|11             rd            7|6    opcode    0|
	J,		// |31  [20]  31|30        imm[10:1]                   21|20  [11]  20|19          imm[19:12]          12|11             rd            7|6    opcode    0|
	INVALID
};

inline std::string to_string(InstructionType type) {
	switch (type) {
	case InstructionType::R:
		return "R";

	case InstructionType::I:
		return "I";

	case InstructionType::S:
		return "S";

	case InstructionType::B:
		return "B";

	case InstructionType::U:
		return "U";

	case InstructionType::J:
		return "J";

	case InstructionType::INVALID:
		return "INVALID";

	default:
		return "unknown type";
	}
}

// RV32I
enum class InstructionName {
	LUI,		// U	opcode=0110111
	AUIPC,		// U	opcode=0010111
	JAL,		// J	opcode=1101111
	JALR,		// I	opcode=1100111		funct3=000
	BEQ,		// B	opcode=1100011		funct3=000
	BNE,		// B	opcode=1100011		funct3=001
	BLT,		// B	opcode=1100011		funct3=100
	BGE,		// B	opcode=1100011		funct3=101
	BLTU,		// B	opcode=1100011		funct3=110
	BGEU,		// B	opcode=1100011		funct3=111
	LB,			// I	opcode=0000011		funct3=000
	LH,			// I	opcode=0000011		funct3=001
	LW,			// I	opcode=0000011		funct3=010
	LBU,		// I	opcode=0000011		funct3=100
	LHU,		// I	opcode=0000011		funct3=101
	SB,			// S	opcode=0100011		funct3=000
	SH,			// S	opcode=0100011		funct3=001
	SW,			// S	opcode=0100011		funct3=010
	ADDI,		// I	opcode=0010011		funct3=000
	SLTI,		// I	opcode=0010011		funct3=010
	SLTIU,		// I	opcode=0010011		funct3=011
	XORI,		// I	opcode=0010011		funct3=100
	ORI,		// I	opcode=0010011		funct3=110
	ANDI,		// I	opcode=0010011		funct3=111
	SLLI,		// I	opcode=0010011		funct3=001							imm=[0000000:shamt]
	SRLI,		// I	opcode=0010011		funct3=101							imm=[0000000:shamt]
	SRAI,		// I	opcode=0010011		funct3=101							imm=[0100000:shamt]
	ADD,		// R	opcode=0110011		funct3=000		funct7=0000000
	SUB,		// R	opcode=0110011		funct3=000		funct7=0100000
	SLL,		// R	opcode=0110011		funct3=001		funct7=0000000
	SLT,		// R	opcode=0110011		funct3=010		funct7=0000000
	SLTU,		// R	opcode=0110011		funct3=011		funct7=0000000
	XOR,		// R	opcode=0110011		funct3=100		funct7=0000000
	SRL,		// R	opcode=0110011		funct3=101		funct7=0000000
	SRA,		// R	opcode=0110011		funct3=101		funct7=0100000
	OR,			// R	opcode=0110011		funct3=110		funct7=0000000
	AND,		// R	opcode=0110011		funct3=111		funct7=0000000
	FENCE,		// I	opcode=0001111		funct3=000							imm=[fm:pred:succ]										// nop
	FENCE_TSO,	// I	opcode=0001111		funct3=000							imm=[1000:0011:0011]		rs1=00000		rd=00000	// nop, is variant of fence
	PAUSE,		// I	opcode=0001111		funct3=000							imm=[0000:0001:0000]		rs1=00000		rd=00000	// nop
	ECALL,		// I	opcode=1110011		funct3=000							imm=[000000000000]			rs1=00000		rd=00000	// raise environment-call exception
	EBREAK,		// I	opcode=1110011		funct3=000							imm=[000000000001]			rs1=00000		rd=00000	// request breakpoint exception
	INVALID
};

inline std::string to_string(InstructionName name) {
	switch (name) {
	case InstructionName::LUI:
		return "LUI";

	case InstructionName::AUIPC:
		return "AUIPC";

	case InstructionName::JAL:
		return "JAL";

	case InstructionName::JALR:
		return "JALR";

	case InstructionName::BEQ:
		return "BEQ";

	case InstructionName::BNE:
		return "BNE";

	case InstructionName::BLT:
		return "BLT";

	case InstructionName::BGE:
		return "BGE";

	case InstructionName::BLTU:
		return "BLTU";

	case InstructionName::BGEU:
		return "BGEU";

	case InstructionName::LB:
		return "LB";

	case InstructionName::LH:
		return "LH";

	case InstructionName::LW:
		return "LW";

	case InstructionName::LBU:
		return "LBU";

	case InstructionName::LHU:
		return "LHU";

	case InstructionName::SB:
		return "SB";

	case InstructionName::SH:
		return "SH";

	case InstructionName::SW:
		return "SW";

	case InstructionName::ADDI:
		return "ADDI";

	case InstructionName::SLTI:
		return "SLTI";

	case InstructionName::SLTIU:
		return "SLTIU";

	case InstructionName::XORI:
		return "XORI";

	case InstructionName::ORI:
		return "ORI";

	case InstructionName::ANDI:
		return "ANDI";

	case InstructionName::SLLI:
		return "SLLI";

	case InstructionName::SRLI:
		return "SRLI";

	case InstructionName::SRAI:
		return "SRAI";

	case InstructionName::ADD:
		return "ADD";

	case InstructionName::SUB:
		return "SUB";

	case InstructionName::SLL:
		return "SLL";

	case InstructionName::SLT:
		return "SLT";

	case InstructionName::SLTU:
		return "SLTU";

	case InstructionName::XOR:
		return "XOR";

	case InstructionName::SRL:
		return "SRL";

	case InstructionName::SRA:
		return "SRA";

	case InstructionName::OR:
		return "OR";

	case InstructionName::AND:
		return "AND";

	case InstructionName::FENCE:
		return "FENCE";

	case InstructionName::FENCE_TSO:
		return "FENCE.TSO";

	case InstructionName::PAUSE:
		return "PAUSE";

	case InstructionName::ECALL:
		return "ECALL";

	case InstructionName::EBREAK:
		return "EBREAK";

	case InstructionName::INVALID:
		return "INVALID";

	default:
		return "unknown name";
	}
}

// represents the decoded instruction and its parts
struct Instruction {
	uint32_t raw{0};									// unnecessary, for debug only

	InstructionType type{InstructionType::INVALID};		// unnecessary, for debug only
	InstructionName name{InstructionName::INVALID};

	uint8_t funct3{0};									// unnecessary, for debug only
	uint8_t funct7{0};									// unnecessary, for debug only
	uint8_t opcode{0};									// unnecessary, for debug only

	uint8_t rd{0};
	uint8_t rs1{0};
	uint8_t rs2{0};

	uint32_t imm{0};

	Instruction(void) = default;
};

inline std::string fence_set_to_string(uint32_t set) {
	std::string str = "";
	if (set & 8) {
		str += 'i';
	}
	if (set & 4) {
		str += 'o';
	}
	if (set & 2) {
		str += 'r';
	}
	if (set & 1) {
		str += 'w';
	}
	return str.empty() ? std::string("0") : str;
}

inline std::string to_string(const Instruction& inst) {
    if (inst.type == InstructionType::INVALID || inst.name == InstructionName::INVALID) {
        return std::format(".word 0x{:08X} invalid", inst.raw);
    }

    std::string mnemonic = to_string(inst.name);
    int32_t signed_imm = static_cast<int32_t>(inst.imm);

    switch (inst.name) {
    case InstructionName::SLLI:
    case InstructionName::SRLI:
    case InstructionName::SRAI:
        return std::format("{} x{}, x{}, {}", mnemonic, inst.rd, inst.rs1, inst.imm);

    case InstructionName::LB:
    case InstructionName::LH:
    case InstructionName::LW:
    case InstructionName::LBU:
    case InstructionName::LHU:
    case InstructionName::JALR:
        return std::format("{} x{}, {}(x{})", mnemonic, inst.rd, signed_imm, inst.rs1);

    case InstructionName::FENCE: {
        return std::format("{} {}, {}", mnemonic, fence_set_to_string((inst.imm >> 4) & 0xF), fence_set_to_string(inst.imm & 0xF));
    }

    case InstructionName::ECALL:
    case InstructionName::EBREAK:
    case InstructionName::PAUSE:
    case InstructionName::FENCE_TSO:
        return mnemonic;

    // exhaustive I guess
    case InstructionName::LUI:
    case InstructionName::AUIPC:
    case InstructionName::JAL:
    case InstructionName::BEQ:
    case InstructionName::BNE:
    case InstructionName::BLT:
    case InstructionName::BGE:
    case InstructionName::BLTU:
    case InstructionName::BGEU:
    case InstructionName::SB:
    case InstructionName::SH:
    case InstructionName::SW:
    case InstructionName::ADDI:
    case InstructionName::SLTI:
    case InstructionName::SLTIU:
    case InstructionName::XORI:
    case InstructionName::ORI:
    case InstructionName::ANDI:
    case InstructionName::ADD:
    case InstructionName::SUB:
    case InstructionName::SLL:
    case InstructionName::SLT:
    case InstructionName::SLTU:
    case InstructionName::XOR:
    case InstructionName::SRL:
    case InstructionName::SRA:
    case InstructionName::OR:
    case InstructionName::AND:
    case InstructionName::INVALID:
    default:
        break;
    }

    switch (inst.type) {
    case InstructionType::R:
        return std::format("{} x{}, x{}, x{}", mnemonic, inst.rd, inst.rs1, inst.rs2);

    case InstructionType::I:
        return std::format("{} x{}, x{}, {}", mnemonic, inst.rd, inst.rs1, signed_imm);

    case InstructionType::S:
        return std::format("{} x{}, {}(x{})", mnemonic, inst.rs2, signed_imm, inst.rs1);

    case InstructionType::B:
        return std::format("{} x{}, x{}, {:+}", mnemonic, inst.rs1, inst.rs2, signed_imm);

    case InstructionType::U:
        return std::format("{} x{}, 0x{:X}", mnemonic, inst.rd, inst.imm >> 12);

    case InstructionType::J:
        return std::format("{} x{}, {:+}", mnemonic, inst.rd, signed_imm);

    case InstructionType::INVALID:
        break;
    }

    return std::format(".word 0x{:08X}", inst.raw);
}

class Decoder {
private:
	static void parse_R(uint32_t raw, Instruction& inst);
	static void parse_I(uint32_t raw, Instruction& inst);
	static void parse_S(uint32_t raw, Instruction& inst);
	static void parse_B(uint32_t raw, Instruction& inst);
	static void parse_U(uint32_t raw, Instruction& inst);
	static void parse_J(uint32_t raw, Instruction& inst);

	static uint8_t extract_funct3(uint32_t raw);
	static uint8_t extract_funct7(uint32_t raw);
	static uint8_t extract_opcode(uint32_t raw);
	static uint8_t extract_rd(uint32_t raw);
	static uint8_t extract_rs1(uint32_t raw);
	static uint8_t extract_rs2(uint32_t raw);
	
	static void validate_R_arithmetic(Instruction& inst);
	static void validate_load(Instruction& inst);
	static void validate_I_arithmetic(Instruction& inst);
	static void validate_misc_mem(Instruction& inst);
	static void validate_system(Instruction& inst);
	static void validate_store(Instruction& inst);
	static void validate_branch(Instruction& inst);

public:
	Decoder(void) = default;
	~Decoder(void) = default;

	Instruction decode_instruction(uint32_t raw);
};

#endif
