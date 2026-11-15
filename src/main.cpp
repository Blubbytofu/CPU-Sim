
// #include <cassert>
// #include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "instruction-set-simulator/core.hpp"

int main() {
    const int MAX_STEPS = 1000;
    std::vector<uint32_t> test_program = {
        0b000000000001'00000'000'00001'0010011u,        // ADDI x1, x0, 1
        0b000000000110'00000'000'00010'0010011u,        // ADDI x2, x0, 6
        0b000000000000'00000'000'00011'0010011u,        // ADDI x3, x0, 0
        0b0000000'00001'00011'000'00011'0110011u,       // ADD x3, x3, x1
        0b000000000001'00001'000'00001'0010011u,        // ADDI x1, x1, 1
        0b1'111111'00010'00001'001'1100'1'1100011u,     // BNE x1, x2, -8
        0b000001000000'00000'000'00100'0010011u,        // ADDI x4, x0, 64
        0b0000000'00011'00100'010'00000'0100011u,       // SW x3, 0(x4)
        0b000000000000'00100'010'00101'0000011u,        // LW x5, 0(x4)

        0b000000000001'00000'000'00000'1110011u         // EBREAK, representing program end for now
    };

    ISSCPU cpu{};
    cpu.load_program(0x1000, test_program);
    cpu.reset(0x1000);

    for (int i = 0; i < MAX_STEPS; i++) {
        std::cout << cpu.expose_state() << "\n";
        ExecutionStep st = cpu.step();
        std::cout << to_string(st) << "\n";
        if (st.trap) {
            break;
        }
    }
    std::cout << cpu.expose_state() << "\n";

    return 0;
}
