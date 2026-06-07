# MIPS to Binary to MIPS

This is a simple converter that helps to encode MIPS assembly instructions to binary and also decode binary machine code into MIPS instructions.

## What is MIPS

[MIPS](https://en.wikipedia.org/wiki/MIPS_architecture) (Microprocessor without Interlocked Pipelined Stages) is a reduced instruction set computer (RISC) instruction set architecture (ISA) developed by MIPS Computer Systems, now MIPS Technologies, based in the United States.

I made this as a fun project for my [Computer Organisation](https://www.comp.nus.edu.sg/~cs2100/) module (CS2100) taught by Dr Colin Tan and Aaron Tan :D to better familiarise myself with it.

## Features

- Decode binary/hex machine code into MIPS instructions
- Encode MIPS instructions into hex machine code
- Automatic pseudo-instruction expansion with detailed explanations
- Support for 6 common MIPS pseudo-instructions (ble, blt, bge, bgt, move, li)

## Supported Instructions

### Real MIPS Instructions

**R-Type Instructions:**
- add, addu, and, jr, nor, or, slt, sltu, sll, srl, sub, subu

**I-Type Instructions:**
- addi, addiu, andi, beq, bne, lbu, lhu, ll, lui, lw, ori, slti, sltiu, sb, sc, sh, sw

**J-Type Instructions:**
- j, jal

### Pseudo-Instructions

The converter automatically expands pseudo-instructions into their equivalent real MIPS instructions:

| Pseudo-Instruction | Expands To | Description |
|-------------------|------------|-------------|
| `ble $rs, $rt, label` | `slt $at, $rt, $rs`<br>`beq $at, $0, label` | Branch if Less Than or Equal |
| `blt $rs, $rt, label` | `slt $at, $rs, $rt`<br>`bne $at, $0, label` | Branch if Less Than |
| `bge $rs, $rt, label` | `slt $at, $rs, $rt`<br>`beq $at, $0, label` | Branch if Greater Than or Equal |
| `bgt $rs, $rt, label` | `slt $at, $rt, $rs`<br>`bne $at, $0, label` | Branch if Greater Than |
| `move $rd, $rs` | `add $rd, $rs, $0` | Copy register value |
| `li $rt, imm` | `ori $rt, $0, imm`<br>or<br>`lui $rt, upper`<br>`ori $rt, $rt, lower` | Load Immediate (1 or 2 instructions based on value) |

## Example Output

### Pseudo-Instruction Expansion

```
Input: ble $2, $3, target

Output:
Pseudo-instruction detected: ble
Expanding 'ble $2, $3, target' into:

  1) slt $at, $3, $2
     # Set $at = 1 if $3 < $2, else 0
  2) beq $at, $0, target
     # Branch if $at == 0 (i.e., $2 <= $3)

BINARY EQUIVALENT:
  slt $at, $3, $2: 00000000011000100000100000101010
  beq $at, $0, target: 00010000001000000000000000000000

HEX EQUIVALENT:
  slt $at, $3, $2: 0062082A
  beq $at, $0, target: 10200000
  Note: Label 'target' uses placeholder offset 0
```

### Regular Instruction Conversion

```
Input: add $t0, $t1, $t2

Output:
MIPS: add $t0, $t1, $t2
BINARY: 00000001001010100100000000100000
HEX: 012A4020
```

## Reference Data

![MIPS Reference Data](./res/mips-reference-data.png)
