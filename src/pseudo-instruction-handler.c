#include "pseudo-instruction-handler.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int isPseudoInstruction(char *instruction) {
    return strcmp(instruction, "ble") == 0 ||
           strcmp(instruction, "blt") == 0 ||
           strcmp(instruction, "bge") == 0 ||
           strcmp(instruction, "bgt") == 0 ||
           strcmp(instruction, "move") == 0 ||
           strcmp(instruction, "li") == 0;
}

void expandPseudoInstruction(ParsedInstruction *parsed, PseudoExpansion *expansion) {
    expansion->isPseudo = 1;
    expansion->numExpanded = 0;

    char *inst = parsed->instruction;
    char *rs = parsed->operands[0];
    char *rt = parsed->operands[1];
    char *label = parsed->operands[2];

    // ble $rs, $rt, label → slt $at, $rt, $rs + beq $at, $0, label
    if (strcmp(inst, "ble") == 0) {
        sprintf(expansion->expandedInstructions[0], "slt $at, %s, %s", rt, rs);
        sprintf(expansion->explanations[0], "Set $at = 1 if %s < %s, else 0", rt, rs);
        sprintf(expansion->expandedInstructions[1], "beq $at, $0, %s", label);
        sprintf(expansion->explanations[1], "Branch if $at == 0 (i.e., %s <= %s)", rs, rt);
        expansion->numExpanded = 2;
    }
    // blt $rs, $rt, label → slt $at, $rs, $rt + bne $at, $0, label
    else if (strcmp(inst, "blt") == 0) {
        sprintf(expansion->expandedInstructions[0], "slt $at, %s, %s", rs, rt);
        sprintf(expansion->explanations[0], "Set $at = 1 if %s < %s, else 0", rs, rt);
        sprintf(expansion->expandedInstructions[1], "bne $at, $0, %s", label);
        sprintf(expansion->explanations[1], "Branch if $at != 0 (i.e., %s < %s)", rs, rt);
        expansion->numExpanded = 2;
    }
    // bge $rs, $rt, label → slt $at, $rs, $rt + beq $at, $0, label
    else if (strcmp(inst, "bge") == 0) {
        sprintf(expansion->expandedInstructions[0], "slt $at, %s, %s", rs, rt);
        sprintf(expansion->explanations[0], "Set $at = 1 if %s < %s, else 0", rs, rt);
        sprintf(expansion->expandedInstructions[1], "beq $at, $0, %s", label);
        sprintf(expansion->explanations[1], "Branch if $at == 0 (i.e., %s >= %s)", rs, rt);
        expansion->numExpanded = 2;
    }
    // bgt $rs, $rt, label → slt $at, $rt, $rs + bne $at, $0, label
    else if (strcmp(inst, "bgt") == 0) {
        sprintf(expansion->expandedInstructions[0], "slt $at, %s, %s", rt, rs);
        sprintf(expansion->explanations[0], "Set $at = 1 if %s < %s, else 0", rt, rs);
        sprintf(expansion->expandedInstructions[1], "bne $at, $0, %s", label);
        sprintf(expansion->explanations[1], "Branch if $at != 0 (i.e., %s > %s)", rs, rt);
        expansion->numExpanded = 2;
    }
    // move $rd, $rs → add $rd, $rs, $0
    else if (strcmp(inst, "move") == 0) {
        char *rd = parsed->operands[0];
        char *rs_move = parsed->operands[1];
        sprintf(expansion->expandedInstructions[0], "add %s, %s, $0", rd, rs_move);
        sprintf(expansion->explanations[0], "Copy %s to %s (add %s + 0)", rs_move, rd, rs_move);
        expansion->numExpanded = 1;
    }
    // li $rt, imm → handle based on immediate size
    else if (strcmp(inst, "li") == 0) {
        char *rd = parsed->operands[0];
        int imm = atoi(parsed->operands[1]);

        // If immediate fits in 16 bits, use ori
        if (imm >= -32768 && imm <= 65535) {
            sprintf(expansion->expandedInstructions[0], "ori %s, $0, %d", rd, imm);
            sprintf(expansion->explanations[0], "Load immediate %d into %s", imm, rd);
            expansion->numExpanded = 1;
        } else {
            // Need lui + ori for 32-bit immediate
            unsigned int uimm = (unsigned int)imm;
            int upper = (uimm >> 16) & 0xFFFF;
            int lower = uimm & 0xFFFF;
            sprintf(expansion->expandedInstructions[0], "lui %s, %d", rd, upper);
            sprintf(expansion->explanations[0], "Load upper 16 bits (%d) into %s", upper, rd);
            sprintf(expansion->expandedInstructions[1], "ori %s, %s, %d", rd, rd, lower);
            sprintf(expansion->explanations[1], "OR lower 16 bits (%d) into %s", lower, rd);
            expansion->numExpanded = 2;
        }
    }
}
