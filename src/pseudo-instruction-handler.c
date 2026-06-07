#include "pseudo-instruction-handler.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int isPseudoInstruction(char *instruction) {
    // Issue #3: Add NULL pointer check
    if (instruction == NULL) {
        return 0;
    }

    return strcmp(instruction, "ble") == 0 ||
           strcmp(instruction, "blt") == 0 ||
           strcmp(instruction, "bge") == 0 ||
           strcmp(instruction, "bgt") == 0 ||
           strcmp(instruction, "move") == 0 ||
           strcmp(instruction, "li") == 0;
}

void expandPseudoInstruction(ParsedInstruction *parsed, PseudoExpansion *expansion) {
    // Issue #3: Add NULL pointer checks
    if (parsed == NULL || expansion == NULL) {
        return;
    }

    expansion->isPseudo = 1;
    expansion->numExpanded = 0;

    char *inst = parsed->instruction;

    // Issue #1: Don't unconditionally access operands - declare but don't assign yet
    char *rs = NULL;
    char *rt = NULL;
    char *label = NULL;

    // ble $rs, $rt, label → slt $at, $rt, $rs + beq $at, $0, label
    if (strcmp(inst, "ble") == 0) {
        // Issue #5: Validate operand count before accessing
        if (parsed->numOperands < 3) {
            return;
        }
        rs = parsed->operands[0];
        rt = parsed->operands[1];
        label = parsed->operands[2];

        // Issue #2: Replace sprintf with snprintf
        snprintf(expansion->expandedInstructions[0], 64, "slt $at, %s, %s", rt, rs);
        snprintf(expansion->explanations[0], 128, "Set $at = 1 if %s < %s, else 0", rt, rs);
        snprintf(expansion->expandedInstructions[1], 64, "beq $at, $0, %s", label);
        snprintf(expansion->explanations[1], 128, "Branch if $at == 0 (i.e., %s <= %s)", rs, rt);
        expansion->numExpanded = 2;
    }
    // blt $rs, $rt, label → slt $at, $rs, $rt + bne $at, $0, label
    else if (strcmp(inst, "blt") == 0) {
        // Issue #5: Validate operand count before accessing
        if (parsed->numOperands < 3) {
            return;
        }
        rs = parsed->operands[0];
        rt = parsed->operands[1];
        label = parsed->operands[2];

        // Issue #2: Replace sprintf with snprintf
        snprintf(expansion->expandedInstructions[0], 64, "slt $at, %s, %s", rs, rt);
        snprintf(expansion->explanations[0], 128, "Set $at = 1 if %s < %s, else 0", rs, rt);
        snprintf(expansion->expandedInstructions[1], 64, "bne $at, $0, %s", label);
        snprintf(expansion->explanations[1], 128, "Branch if $at != 0 (i.e., %s < %s)", rs, rt);
        expansion->numExpanded = 2;
    }
    // bge $rs, $rt, label → slt $at, $rs, $rt + beq $at, $0, label
    else if (strcmp(inst, "bge") == 0) {
        // Issue #5: Validate operand count before accessing
        if (parsed->numOperands < 3) {
            return;
        }
        rs = parsed->operands[0];
        rt = parsed->operands[1];
        label = parsed->operands[2];

        // Issue #2: Replace sprintf with snprintf
        snprintf(expansion->expandedInstructions[0], 64, "slt $at, %s, %s", rs, rt);
        snprintf(expansion->explanations[0], 128, "Set $at = 1 if %s < %s, else 0", rs, rt);
        snprintf(expansion->expandedInstructions[1], 64, "beq $at, $0, %s", label);
        snprintf(expansion->explanations[1], 128, "Branch if $at == 0 (i.e., %s >= %s)", rs, rt);
        expansion->numExpanded = 2;
    }
    // bgt $rs, $rt, label → slt $at, $rt, $rs + bne $at, $0, label
    else if (strcmp(inst, "bgt") == 0) {
        // Issue #5: Validate operand count before accessing
        if (parsed->numOperands < 3) {
            return;
        }
        rs = parsed->operands[0];
        rt = parsed->operands[1];
        label = parsed->operands[2];

        // Issue #2: Replace sprintf with snprintf
        snprintf(expansion->expandedInstructions[0], 64, "slt $at, %s, %s", rt, rs);
        snprintf(expansion->explanations[0], 128, "Set $at = 1 if %s < %s, else 0", rt, rs);
        snprintf(expansion->expandedInstructions[1], 64, "bne $at, $0, %s", label);
        snprintf(expansion->explanations[1], 128, "Branch if $at != 0 (i.e., %s > %s)", rs, rt);
        expansion->numExpanded = 2;
    }
    // move $rd, $rs → add $rd, $rs, $0
    else if (strcmp(inst, "move") == 0) {
        // Issue #5: Validate operand count before accessing
        if (parsed->numOperands < 2) {
            return;
        }
        char *rd = parsed->operands[0];
        char *rs_move = parsed->operands[1];

        // Issue #2: Replace sprintf with snprintf
        snprintf(expansion->expandedInstructions[0], 64, "add %s, %s, $0", rd, rs_move);
        snprintf(expansion->explanations[0], 128, "Copy %s to %s (add %s + 0)", rs_move, rd, rs_move);
        expansion->numExpanded = 1;
    }
    // li $rt, imm → handle based on immediate size
    else if (strcmp(inst, "li") == 0) {
        // Issue #5: Validate operand count before accessing
        if (parsed->numOperands < 2) {
            return;
        }
        char *rd = parsed->operands[0];
        int imm = atoi(parsed->operands[1]);

        // Issue #4: Fix range check for signed 16-bit immediate
        if (imm >= -32768 && imm <= 32767) {
            // Issue #2: Replace sprintf with snprintf
            snprintf(expansion->expandedInstructions[0], 64, "ori %s, $0, %d", rd, imm);
            snprintf(expansion->explanations[0], 128, "Load immediate %d into %s", imm, rd);
            expansion->numExpanded = 1;
        } else {
            // Need lui + ori for 32-bit immediate
            unsigned int uimm = (unsigned int)imm;
            int upper = (uimm >> 16) & 0xFFFF;
            int lower = uimm & 0xFFFF;
            // Issue #2: Replace sprintf with snprintf
            snprintf(expansion->expandedInstructions[0], 64, "lui %s, %d", rd, upper);
            snprintf(expansion->explanations[0], 128, "Load upper 16 bits (%d) into %s", upper, rd);
            snprintf(expansion->expandedInstructions[1], 64, "ori %s, %s, %d", rd, rd, lower);
            snprintf(expansion->explanations[1], 128, "OR lower 16 bits (%d) into %s", lower, rd);
            expansion->numExpanded = 2;
        }
    }
}
