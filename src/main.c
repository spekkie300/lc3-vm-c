#include <stdint.h>
#include <stdio.h>

// function declations
uint16_t sign_extend(uint16_t x, int bit_count);
uint16_t update_flags(uint16_t r);

// Memory definitions and declaration
#define MEMORY_MAX (1 << 16)
uint16_t memory[MEMORY_MAX];

// Registers
enum {
  R_R0 = 0,
  R_R1,
  R_R2,
  R_R3,
  R_R4,
  R_R5,
  R_R6,
  R_R7,
  R_PC,
  R_COND,
  R_COUNT
};
uint16_t reg[R_COUNT];

// Condition flags
enum {
  FL_POS = 1 << 0,  // P
  FL_ZRO = 1 << 1,  // Z
  FL_NEG = 1 << 2,  // N

};

// Opcodes
enum {
  OP_BR = 0,  // BRANCH
  OP_ADD,     // ADD
  OP_LD,      // LOAD
  OP_ST,      // STORE
  OP_JSR,     // JUMP REGISTER
  OP_AND,     // AND
  OP_LDR,     // LOAD BASE + OFFSET
  OP_STR,     // STORE BASE + OFFSET
  OP_RTI,     // UNUSED
  OP_NOT,     // NOT
  OP_LDI,     // LOAD INDIRECT
  OP_STI,     // STORE INDIRECT
  OP_JMP,     // JUMP
  OP_RES,     // RESERVED(UNUSED)
  OP_LEA,     // LOAD EFFECTIVE ADDRESS
  OP_TRAP,    // EXECUTE TRAP

};

int main(int argc, const char* argv[]) {
  if (argc < 2) {
    printf("Usage: ./lc3 [image-file]... \n");
    return 1;
  }

  // for loop to read the image-file
  // TODO

  // Set the Z flag since COND flag needs to be set
  reg[R_COND] = FL_ZRO;

  enum { PC_START = 0x3000 };
  reg[R_PC] = PC_START;

  int running = 1;

  while (running) {
    // Fetch instruction from memory
    uint16_t instr = mem_read(reg[R_PC]++);
    uint16_t op = instr >> 12;

    switch (op) {
      case OP_ADD: {
        // destination register
        uint16_t r0 = (instr >> 9) & 0x7;
        // First operand(SR1)
        uint16_t r1 = (instr >> 6) & 0x7;
        // imm5 flag
        uint16_t imm_flag = (instr >> 5) & 0x1;

        if (imm_flag) {
          uint16_t imm5 = sign_extend(instr & 0x1f, 5);
          reg[r0] = reg[r1] + imm5;
        } else {
          uint16_t r2 = (instr & 0x7);
          reg[r0] = reg[r1] + reg[r2];
        }
        update_flags(r0);
      } break;
      case OP_AND: {
        uint16_t r0 = (instr << 9) & 0x7;
        uint16_t r1 = (instr << 6) & 0x7;
        uint16_t imm_flag = (instr << 5) & 0x1;

        if (imm_flag) {
          uint16_t imm5 = sign_extend(instr & 0x1f, 5);
          reg[r0] = reg[r1] & reg[imm5];
        } else {
          uint16_t r2 = (instr & 0x7);
          reg[r0] = reg[r1] & reg[r2];
        }
        update_flags(r0);
      } break;
      case OP_NOT: {
        uint16_t r0 = (instr << 9) & 0x7;
        uint16_t r1 = (instr << 6) & 0x7;

        reg[r0] = !reg[r1];

        update_flags(r0);
      } break;
      case OP_BR: {
        uint16_t cond_flag = (instr << 9) & 0x7;

        uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);

        if (cond_flag & reg[R_COND]) {
          reg[R_PC] += pc_offset;
        }
      } break;
      case OP_JMP: {
        uint16_t r0 = (instr << 6) & 0x7;

        reg[R_PC] = reg[r0];
      } break;
      case OP_JSR:
        reg[R_R7] = reg[R_PC];
        uint16_t jsrr_flag = (instr << 11) & 0x1;

        if (jsrr_flag) {
          uint16_t pc_offset = sign_extend(instr & 0x7FF, 11);
          reg[R_PC] += pc_offset;
        } else {
          uint16_t r1 = (instr << 6) & 0x7;
          reg[R_PC] = reg[r1];
        }
        break;
      case OP_LD:

        break;
      case OP_LDI: {
        // destination register
        uint16_t r0 = (instr >> 9) & 0x7;
        // Get the PC offset
        uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);

        reg[r0] = mem_read(mem_read(reg[R_PC] + pc_offset));

        update_flags(r0);
      } break;
      case OP_LDR:
        // todo
        break;
      case OP_LEA:
        // todo
        break;
      case OP_ST:
        // todo
        break;
      case OP_STI:
        // todo
        break;
      case OP_STR:
        // todo
        break;
      case OP_TRAP:
        // todo
        break;
      case OP_RES:
      case OP_RTI:
      default:
        // todo
        break;
    }
  }
  // shutdown function
}

uint16_t sign_extend(uint16_t x, int bit_count) {
  if ((x >> (bit_count - 1)) & 1) {
    x |= (0xFFFF << bit_count);
  }
  return x;
}

uint16_t update_flags(uint16_t r) {
  if (reg[r] == 0) {
    reg[R_COND] = FL_ZRO;
  } else if (reg[r] >>
             15) {  // Bitshift r 15 places to check if its a 1(negative)
    reg[R_COND] = FL_NEG;
  } else {
    reg[R_COND] = FL_POS;
  }
}
