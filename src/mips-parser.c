#include "mips-parser.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

int extractRegisterNumber(char *reg) {
    // Handle $0, $1, etc.
    if (reg[1] >= '0' && reg[1] <= '9') {
        return atoi(&reg[1]);
    }

    // Handle $zero, $at, $v0, $a0, $t0, $s0, etc.
    if (strcmp(reg, "$zero") == 0) return 0;
    if (strcmp(reg, "$at") == 0) return 1;
    if (strcmp(reg, "$v0") == 0) return 2;
    if (strcmp(reg, "$v1") == 0) return 3;
    if (strcmp(reg, "$a0") == 0) return 4;
    if (strcmp(reg, "$a1") == 0) return 5;
    if (strcmp(reg, "$a2") == 0) return 6;
    if (strcmp(reg, "$a3") == 0) return 7;
    if (strcmp(reg, "$t0") == 0) return 8;
    if (strcmp(reg, "$t1") == 0) return 9;
    if (strcmp(reg, "$t2") == 0) return 10;
    if (strcmp(reg, "$t3") == 0) return 11;
    if (strcmp(reg, "$t4") == 0) return 12;
    if (strcmp(reg, "$t5") == 0) return 13;
    if (strcmp(reg, "$t6") == 0) return 14;
    if (strcmp(reg, "$t7") == 0) return 15;
    if (strcmp(reg, "$s0") == 0) return 16;
    if (strcmp(reg, "$s1") == 0) return 17;
    if (strcmp(reg, "$s2") == 0) return 18;
    if (strcmp(reg, "$s3") == 0) return 19;
    if (strcmp(reg, "$s4") == 0) return 20;
    if (strcmp(reg, "$s5") == 0) return 21;
    if (strcmp(reg, "$s6") == 0) return 22;
    if (strcmp(reg, "$s7") == 0) return 23;
    if (strcmp(reg, "$t8") == 0) return 24;
    if (strcmp(reg, "$t9") == 0) return 25;
    if (strcmp(reg, "$k0") == 0) return 26;
    if (strcmp(reg, "$k1") == 0) return 27;
    if (strcmp(reg, "$gp") == 0) return 28;
    if (strcmp(reg, "$sp") == 0) return 29;
    if (strcmp(reg, "$fp") == 0) return 30;
    if (strcmp(reg, "$ra") == 0) return 31;

    return 0; // Default to $0 if unknown
}

int parseImmediate(char *imm, int *isLabel) {
    *isLabel = 0;

    // Check if it's a label (starts with letter or contains non-digit chars besides '-')
    int i = 0;
    if (imm[0] == '-') i = 1; // Skip negative sign

    int hasNonDigit = 0;
    for (; imm[i] != '\0'; i++) {
        if (!isdigit(imm[i])) {
            hasNonDigit = 1;
            break;
        }
    }

    if (hasNonDigit) {
        *isLabel = 1;
        return 0; // Return placeholder offset for labels
    }

    return atoi(imm);
}

void parseMipsInstruction(char *mips, ParsedInstruction *output) {
    char temp[128];
    strcpy(temp, mips);

    // Extract instruction (first token)
    char *token = strtok(temp, " ,()");
    strcpy(output->instruction, token);

    output->numOperands = 0;
    output->isLabel = 0;

    // Extract operands
    while ((token = strtok(NULL, " ,()")) != NULL && output->numOperands < MAX_OPERANDS) {
        strcpy(output->operands[output->numOperands], token);
        output->numOperands++;
    }

    // Check if last operand is a label (for branch instructions)
    if (output->numOperands > 0) {
        char *lastOp = output->operands[output->numOperands - 1];
        if (lastOp[0] != '$' && lastOp[0] != '-' && !isdigit(lastOp[0])) {
            output->isLabel = 1;
        }
    }
}
