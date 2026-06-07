#include "mips-to-binary.h"
#include "instruction-code-translator.h"
#include "radix-translator.h"
#include "mips-parser.h"
#include <string.h>
#include <stdlib.h>

void mipsToBinary(ParsedInstruction *parsed, char *binaryOutput) {
    char opCode[7] = {0};
    char type;

    // Get opCode for this instruction
    mipsToOpCode(parsed->instruction, opCode);

    // Determine instruction type
    type = getInstructionType(opCode);

    // Initialize binary output with all zeros
    strcpy(binaryOutput, "00000000000000000000000000000000");

    if (type == 'R') {
        // R-type: opcode(6) rs(5) rt(5) rd(5) shamt(5) funct(6)
        char fnCode[7] = {0};
        char rsBinary[6] = "00000";
        char rtBinary[6] = "00000";
        char rdBinary[6] = "00000";
        char shamtBinary[6] = "00000";

        mipsToFnCode(parsed->instruction, fnCode);

        // Handle different R-type formats
        if (strcmp(parsed->instruction, "jr") == 0) {
            // jr $rs -> rs field only
            int rs = extractRegisterNumber(parsed->operands[0]);
            decimalToBinary(rs, rsBinary);

            // Build: opcode(6) + rs(5) + 00000 + 00000 + 00000 + funct(6)
            strncpy(binaryOutput, opCode, 6);
            strncpy(binaryOutput + 6, rsBinary, 5);
            strncpy(binaryOutput + 11, "00000", 5); // rt
            strncpy(binaryOutput + 16, "00000", 5); // rd
            strncpy(binaryOutput + 21, "00000", 5); // shamt
            strncpy(binaryOutput + 26, fnCode, 6);

        } else if (strcmp(parsed->instruction, "sll") == 0 || strcmp(parsed->instruction, "srl") == 0) {
            // Shift: rd, rt, shamt -> rd, rt, shamt
            int rd = extractRegisterNumber(parsed->operands[0]);
            int rt = extractRegisterNumber(parsed->operands[1]);
            int shamt = atoi(parsed->operands[2]);

            decimalToBinary(rd, rdBinary);
            decimalToBinary(rt, rtBinary);
            decimalToBinary(shamt, shamtBinary);

            // Build: opcode(6) + 00000 + rt(5) + rd(5) + shamt(5) + funct(6)
            strncpy(binaryOutput, opCode, 6);
            strncpy(binaryOutput + 6, "00000", 5); // rs
            strncpy(binaryOutput + 11, rtBinary, 5);
            strncpy(binaryOutput + 16, rdBinary, 5);
            strncpy(binaryOutput + 21, shamtBinary, 5);
            strncpy(binaryOutput + 26, fnCode, 6);

        } else {
            // Standard R-type: rd, rs, rt
            int rd = extractRegisterNumber(parsed->operands[0]);
            int rs = extractRegisterNumber(parsed->operands[1]);
            int rt = extractRegisterNumber(parsed->operands[2]);

            decimalToBinary(rs, rsBinary);
            decimalToBinary(rt, rtBinary);
            decimalToBinary(rd, rdBinary);

            // Build: opcode(6) + rs(5) + rt(5) + rd(5) + shamt(5) + funct(6)
            strncpy(binaryOutput, opCode, 6);
            strncpy(binaryOutput + 6, rsBinary, 5);
            strncpy(binaryOutput + 11, rtBinary, 5);
            strncpy(binaryOutput + 16, rdBinary, 5);
            strncpy(binaryOutput + 21, "00000", 5); // shamt
            strncpy(binaryOutput + 26, fnCode, 6);
        }

    } else if (type == 'I') {
        // I-type: opcode(6) rs(5) rt(5) immediate(16)
        char rsBinary[6] = "00000";
        char rtBinary[6] = "00000";
        char immBinary[17] = "0000000000000000";

        if (strcmp(parsed->instruction, "lw") == 0 || strcmp(parsed->instruction, "sw") == 0 ||
            strcmp(parsed->instruction, "lbu") == 0 || strcmp(parsed->instruction, "lhu") == 0 ||
            strcmp(parsed->instruction, "sb") == 0 || strcmp(parsed->instruction, "sh") == 0 ||
            strcmp(parsed->instruction, "ll") == 0 || strcmp(parsed->instruction, "sc") == 0) {
            // Load/Store: rt, offset(rs) format
            int rt = extractRegisterNumber(parsed->operands[0]);
            int offset = atoi(parsed->operands[1]); // offset is first in operands[1]
            int rs = extractRegisterNumber(parsed->operands[2]); // base register is in operands[2]

            decimalToBinary(rs, rsBinary);
            decimalToBinary(rt, rtBinary);

            // Handle signed offset (16-bit two's complement)
            if (offset < 0) {
                // Convert negative to 16-bit two's complement
                offset = (1 << 16) + offset; // Add 2^16 to get positive representation
            }
            decimalToBinary(offset, immBinary);

            // Build: opcode(6) + rs(5) + rt(5) + immediate(16)
            strncpy(binaryOutput, opCode, 6);
            strncpy(binaryOutput + 6, rsBinary, 5);
            strncpy(binaryOutput + 11, rtBinary, 5);
            strncpy(binaryOutput + 16, immBinary, 16);

        } else if (strcmp(parsed->instruction, "beq") == 0 || strcmp(parsed->instruction, "bne") == 0) {
            // Branch: rs, rt, offset format
            int rs = extractRegisterNumber(parsed->operands[0]);
            int rt = extractRegisterNumber(parsed->operands[1]);
            int offset = 0;

            // Handle offset (could be label or immediate)
            if (!parsed->isLabel) {
                offset = atoi(parsed->operands[2]);

                // Handle signed offset (16-bit two's complement)
                if (offset < 0) {
                    offset = (1 << 16) + offset;
                }
            }

            decimalToBinary(rs, rsBinary);
            decimalToBinary(rt, rtBinary);
            decimalToBinary(offset, immBinary);

            // Build: opcode(6) + rs(5) + rt(5) + immediate(16)
            strncpy(binaryOutput, opCode, 6);
            strncpy(binaryOutput + 6, rsBinary, 5);
            strncpy(binaryOutput + 11, rtBinary, 5);
            strncpy(binaryOutput + 16, immBinary, 16);

        } else if (strcmp(parsed->instruction, "lui") == 0) {
            // lui: rt, immediate format (no rs)
            int rt = extractRegisterNumber(parsed->operands[0]);
            int imm = atoi(parsed->operands[1]);

            decimalToBinary(rt, rtBinary);

            if (imm < 0) {
                imm = (1 << 16) + imm;
            }
            decimalToBinary(imm, immBinary);

            // Build: opcode(6) + 00000 + rt(5) + immediate(16)
            strncpy(binaryOutput, opCode, 6);
            strncpy(binaryOutput + 6, "00000", 5); // rs is 0
            strncpy(binaryOutput + 11, rtBinary, 5);
            strncpy(binaryOutput + 16, immBinary, 16);

        } else {
            // Standard I-type (addi, andi, ori, etc.): rt, rs, immediate
            int rt = extractRegisterNumber(parsed->operands[0]);
            int rs = extractRegisterNumber(parsed->operands[1]);
            int imm = atoi(parsed->operands[2]);

            decimalToBinary(rs, rsBinary);
            decimalToBinary(rt, rtBinary);

            // Handle signed immediate (16-bit two's complement)
            if (imm < 0) {
                imm = (1 << 16) + imm;
            }
            decimalToBinary(imm, immBinary);

            // Build: opcode(6) + rs(5) + rt(5) + immediate(16)
            strncpy(binaryOutput, opCode, 6);
            strncpy(binaryOutput + 6, rsBinary, 5);
            strncpy(binaryOutput + 11, rtBinary, 5);
            strncpy(binaryOutput + 16, immBinary, 16);
        }

    } else if (type == 'J') {
        // J-type: opcode(6) address(26)
        char addressBinary[27] = "00000000000000000000000000";
        int address = 0;

        // Handle address (could be label or immediate)
        if (!parsed->isLabel) {
            address = atoi(parsed->operands[0]);
        }

        decimalToBinary(address, addressBinary);

        // Build: opcode(6) + address(26)
        strncpy(binaryOutput, opCode, 6);
        strncpy(binaryOutput + 6, addressBinary, 26);
    }

    // Ensure null termination
    binaryOutput[32] = '\0';
}
