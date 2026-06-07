#ifndef PSEUDO_INSTRUCTION_HANDLER_H
#define PSEUDO_INSTRUCTION_HANDLER_H

#include "mips-parser.h"

typedef struct {
    int isPseudo;
    int numExpanded;
    char expandedInstructions[2][64]; // Max 2 instructions for expansion
    char explanations[2][128]; // Explanation for each expanded instruction
} PseudoExpansion;

int isPseudoInstruction(char *instruction);
void expandPseudoInstruction(ParsedInstruction *parsed, PseudoExpansion *expansion);

#endif
