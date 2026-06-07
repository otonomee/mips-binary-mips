#ifndef MIPS_PARSER_H
#define MIPS_PARSER_H

#define MAX_INSTRUCTION_LENGTH 10
#define MAX_OPERANDS 3

typedef struct {
    char instruction[MAX_INSTRUCTION_LENGTH];
    int numOperands;
    char operands[MAX_OPERANDS][32]; // Store operands as strings (e.g., "$9", "12", "End")
    int isLabel; // 1 if last operand is a label (non-numeric)
} ParsedInstruction;

void parseMipsInstruction(char *mips, ParsedInstruction *output);
int extractRegisterNumber(char *reg); // Extracts number from "$9" or "$t0"
int parseImmediate(char *imm, int *isLabel); // Parse immediate/offset, sets isLabel if symbolic

#endif
