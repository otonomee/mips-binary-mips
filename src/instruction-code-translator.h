#ifndef INSTRUCTION_CODE_TRANSLATOR_H_
#define INSTRUCTION_CODE_TRANSLATOR_H_

void getOpCode(char*, char*);
void getFnCode(char*, char*);
char getInstructionType(char*);
void getInstruction(char*, char*, char*);
void mipsToOpCode(char *instruction, char *output);
void mipsToFnCode(char *instruction, char *output);

#endif