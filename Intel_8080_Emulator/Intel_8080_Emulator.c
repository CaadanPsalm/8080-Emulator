#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h> 

/*
ConditionCodes - struct for the flags that are used for opcodes.

z - Zero bit (1 if result of instruction is 0 | 0 if result of instruction is not 0)
s - Sign bit (1 if a given number is from -128 to -1 | 0 if a given number is 0 to 127)
p - Parity bit (1 if the number of '1' bits are even | 0 if the nunmber of '1' bits are odd)
cy - Carry bit (Used by operations that use carries, like addition or subtraction.)
ac - Auxillary Carry bit (Special carry bit used only for the DAA operation. A carry bit for the 3rd bit.)
pad - a padding for the struct. Keeps the struct clean and for matching the actual 8080 processor that only uses the 5 bits.
*/
typedef struct ConditionCodes {
    uint8_t z:1;
    uint8_t s:1;
    uint8_t p:1;
    uint8_t cy:1;
    uint8_t ac:1;
    uint8_t pad:3;
} ConditionCodes;

/*
State8080 - struct for the registers and state of the 8080 processor.

a, b, c, d, e, h, l - Registers that are used for operations.
sp - Stack Pointer. Points to the top of the stack and is used for PUSH, POP, CALL or RET operations.
pc - Program Counter. Points to the next address in the memory of the next instruction for the CPU. Used for the disassembler.
memory - Memory. Emulates the 8080's 16 bit memory space.
cc - Condition Codes. Stores the condition from the struct.
int_enable - Interrupt Enable Flag. Used by the opcodes 'EI' and 'DI' for signalling interrupts to the CPU.
*/
typedef struct State8080 {
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    uint16_t sp;
    uint16_t pc;
    uint8_t *memory;
    struct ConditionCodes cc;
    uint8_t int_enable;
} State8080;

/*
Disassemble8080OpCode - function that prints the operations made by the CPU according to 8080 opcodes.

codebuffer -  a valid pointer to 8080 hex opcode
pc - offset into the code

returns the number of bytes of the opcode
*/
int Disassemble8080OpCode (unsigned char *codebuffer, int pc) {
    unsigned char *code = &codebuffer[pc];
    int opbytes = 1;
    printf("%04X ", pc);
    switch(*code)
    {
        case 0x00: printf("NOP"); break;
        case 0x01: printf("LXI   B, #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0x02: printf("STAX  B"); break;
        case 0x03: printf("INX   B"); break;
        case 0x04: printf("INR   B"); break;
        case 0x05: printf("DCR   B"); break;
        case 0x06: printf("MVI   B, #%02X", code[1]); opbytes = 2; break;
        case 0x07: printf("RLC"); break;
        case 0x08: printf("NOP"); break;
        case 0x09: printf("DAD   B"); break;
        case 0x0a: printf("LDAX  B"); break;
        case 0x0b: printf("DCX   B"); break;
        case 0x0c: printf("INR   C"); break;
        case 0x0d: printf("DCR   C"); break;
        case 0x0e: printf("MVI   C, #%02X", code[1]); opbytes = 2; break;
        case 0x0f: printf("RRC"); break;

        case 0x10: printf("NOP"); break;
        case 0x11: printf("LXI   D, #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0x12: printf("STAX  D"); break;
        case 0x13: printf("INX   D"); break;
        case 0x14: printf("INR   D"); break;
        case 0x15: printf("DCR   D"); break;
        case 0x16: printf("MVI   D, #%02X", code[1]); opbytes = 2; break;
        case 0x17: printf("RAL"); break;
        case 0x18: printf("NOP"); break;
        case 0x19: printf("DAD   D"); break;
        case 0x1a: printf("LDAX  D"); break;
        case 0x1b: printf("DCX   D"); break;
        case 0x1c: printf("INR   E"); break;
        case 0x1d: printf("DCR   E"); break;
        case 0x1e: printf("MVI   E, #%02X", code[1]); opbytes = 2; break;
        case 0x1f: printf("RAR"); break;

        case 0x20: printf("RIM"); break;
        case 0x21: printf("LXI   H, #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0x22: printf("SHLD  #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0x23: printf("INX   H"); break;
        case 0x24: printf("INR   H"); break;
        case 0x25: printf("DCR   H"); break;
        case 0x26: printf("MVI   H, #%02X", code[1]); opbytes = 2; break;
        case 0x27: printf("DAA"); break;
        case 0x28: printf("NOP"); break;
        case 0x29: printf("DAD   H"); break;
        case 0x2a: printf("LHLD  #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0x2b: printf("DCX   H"); break;
        case 0x2c: printf("INR   L"); break;
        case 0x2d: printf("DCR   L"); break;
        case 0x2e: printf("MVI   L, #%02X", code[1]); opbytes = 2; break;
        case 0x2f: printf("CMA"); break;

        case 0x30: printf("NOP"); break;
        case 0x31: printf("LXI   SP, #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0x32: printf("STA   #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0x33: printf("INX   SP"); break;
        case 0x34: printf("INR   M"); break;
        case 0x35: printf("DCR   M"); break;
        case 0x36: printf("MVI   M, #%02X", code[1]); opbytes = 2; break;
        case 0x37: printf("STC"); break;
        case 0x38: printf("NOP"); break;
        case 0x39: printf("DAD   SP"); break;
        case 0x3a: printf("LDA   #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0x3b: printf("DCX   SP"); break;
        case 0x3c: printf("INR   A"); break;
        case 0x3d: printf("DCR   A"); break;
        case 0x3e: printf("MVI   A, #%02X", code[1]); opbytes = 2; break;
        case 0x3f: printf("CMC"); break;

        case 0x40: printf("MOV   B, B"); break;
        case 0x41: printf("MOV   B, C"); break;
        case 0x42: printf("MOV   B, D"); break;
        case 0x43: printf("MOV   B, E"); break;
        case 0x44: printf("MOV   B, H"); break;
        case 0x45: printf("MOV   B, L"); break;
        case 0x46: printf("MOV   B, M"); break;
        case 0x47: printf("MOV   B, A"); break;
        case 0x48: printf("MOV   C, B"); break;
        case 0x49: printf("MOV   C, C"); break;
        case 0x4a: printf("MOV   C, D"); break;
        case 0x4b: printf("MOV   C, E"); break;
        case 0x4c: printf("MOV   C, H"); break;
        case 0x4d: printf("MOV   C, L"); break;
        case 0x4e: printf("MOV   C, M"); break;
        case 0x4f: printf("MOV   C, A"); break;

        case 0x50: printf("MOV   D, B"); break;
        case 0x51: printf("MOV   D, C"); break;
        case 0x52: printf("MOV   D, D"); break;
        case 0x53: printf("MOV   D, E"); break;
        case 0x54: printf("MOV   D, H"); break;
        case 0x55: printf("MOV   D, L"); break;
        case 0x56: printf("MOV   D, M"); break;
        case 0x57: printf("MOV   D, A"); break;
        case 0x58: printf("MOV   E, B"); break;
        case 0x59: printf("MOV   E, C"); break;
        case 0x5a: printf("MOV   E, D"); break;
        case 0x5b: printf("MOV   E, E"); break;
        case 0x5c: printf("MOV   E, H"); break;
        case 0x5d: printf("MOV   E, L"); break;
        case 0x5e: printf("MOV   E, M"); break;
        case 0x5f: printf("MOV   E, A"); break;

        case 0x60: printf("MOV   H, B"); break;
        case 0x61: printf("MOV   H, C"); break;
        case 0x62: printf("MOV   H, D"); break;
        case 0x63: printf("MOV   H, E"); break;
        case 0x64: printf("MOV   H, H"); break;
        case 0x65: printf("MOV   H, L"); break;
        case 0x66: printf("MOV   H, M"); break;
        case 0x67: printf("MOV   H, A"); break;
        case 0x68: printf("MOV   L, B"); break;
        case 0x69: printf("MOV   L, C"); break;
        case 0x6a: printf("MOV   L, D"); break;
        case 0x6b: printf("MOV   L, E"); break;
        case 0x6c: printf("MOV   L, H"); break;
        case 0x6d: printf("MOV   L, L"); break;
        case 0x6e: printf("MOV   L, M"); break;
        case 0x6f: printf("MOV   L, A"); break;

        case 0x70: printf("MOV   M, B"); break;
        case 0x71: printf("MOV   M, C"); break;
        case 0x72: printf("MOV   M, D"); break;
        case 0x73: printf("MOV   M, E"); break;
        case 0x74: printf("MOV   M, H"); break;
        case 0x75: printf("MOV   M, L"); break;
        case 0x76: printf("HLT"); break;
        case 0x77: printf("MOV   M, A"); break;
        case 0x78: printf("MOV   A, B"); break;
        case 0x79: printf("MOV   A, C"); break;
        case 0x7a: printf("MOV   A, D"); break;
        case 0x7b: printf("MOV   A, E"); break;
        case 0x7c: printf("MOV   A, H"); break;
        case 0x7d: printf("MOV   A, L"); break;
        case 0x7e: printf("MOV   A, M"); break;
        case 0x7f: printf("MOV   A, A"); break;

        case 0x80: printf("ADD   B"); break;
        case 0x81: printf("ADD   C"); break;
        case 0x82: printf("ADD   D"); break;
        case 0x83: printf("ADD   E"); break;
        case 0x84: printf("ADD   H"); break;
        case 0x85: printf("ADD   L"); break;
        case 0x86: printf("ADD   M"); break;
        case 0x87: printf("ADD   A"); break;
        case 0x88: printf("ADC   B"); break;
        case 0x89: printf("ADC   C"); break;
        case 0x8a: printf("ADC   D"); break;
        case 0x8b: printf("ADC   E"); break;
        case 0x8c: printf("ADC   H"); break;
        case 0x8d: printf("ADC   L"); break;
        case 0x8e: printf("ADC   M"); break;
        case 0x8f: printf("ADC   A"); break;

        case 0x90: printf("SUB   B"); break;
        case 0x91: printf("SUB   C"); break;
        case 0x92: printf("SUB   D"); break;
        case 0x93: printf("SUB   E"); break;
        case 0x94: printf("SUB   H"); break;
        case 0x95: printf("SUB   L"); break;
        case 0x96: printf("SUB   M"); break;
        case 0x97: printf("SUB   A"); break;
        case 0x98: printf("SBB   B"); break;
        case 0x99: printf("SBB   C"); break;
        case 0x9a: printf("SBB   D"); break;
        case 0x9b: printf("SBB   E"); break;
        case 0x9c: printf("SBB   H"); break;
        case 0x9d: printf("SBB   L"); break;
        case 0x9e: printf("SBB   M"); break;
        case 0x9f: printf("SBB   A"); break;

        case 0xa0: printf("ANA   B"); break;
        case 0xa1: printf("ANA   C"); break;
        case 0xa2: printf("ANA   D"); break;
        case 0xa3: printf("ANA   E"); break;
        case 0xa4: printf("ANA   H"); break;
        case 0xa5: printf("ANA   L"); break;
        case 0xa6: printf("ANA   M"); break;
        case 0xa7: printf("ANA   A"); break;
        case 0xa8: printf("XRA   B"); break;
        case 0xa9: printf("XRA   C"); break;
        case 0xaa: printf("XRA   D"); break;
        case 0xab: printf("XRA   E"); break;
        case 0xac: printf("XRA   H"); break;
        case 0xad: printf("XRA   L"); break;
        case 0xae: printf("XRA   M"); break;
        case 0xaf: printf("XRA   A"); break;

        case 0xb0: printf("ORA   B"); break;
        case 0xb1: printf("ORA   C"); break;
        case 0xb2: printf("ORA   D"); break;
        case 0xb3: printf("ORA   E"); break;
        case 0xb4: printf("ORA   H"); break;
        case 0xb5: printf("ORA   L"); break;
        case 0xb6: printf("ORA   M"); break;
        case 0xb7: printf("CMP   A"); break;
        case 0xb8: printf("CMP   B"); break;
        case 0xb9: printf("CMP   C"); break;
        case 0xba: printf("CMP   D"); break;
        case 0xbb: printf("CMP   E"); break;
        case 0xbc: printf("CMP   H"); break;
        case 0xbd: printf("CMP   L"); break;
        case 0xbe: printf("CMP   M"); break;
        case 0xbf: printf("CMP   A"); break;

        case 0xc0: printf("RNZ"); break;
        case 0xc1: printf("POP   B"); break;
        case 0xc2: printf("JNZ   #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0xc3: printf("JMP   #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0xc4: printf("CNZ   #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0xc5: printf("PUSH  B"); break;
        case 0xc6: printf("ADI   #%02X", code[1]); opbytes = 2; break;
        case 0xc7: printf("RST   0"); break;
        case 0xc8: printf("RZ"); break;
        case 0xc9: printf("RET"); break;
        case 0xca: printf("JZ    #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0xcb: printf("JMP   #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0xcc: printf("CZ    #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0xcd: printf("CALL  #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0xce: printf("ACI   #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0xcf: printf("RST   1"); break;

        case 0xd0: printf("RNC"); break;
        case 0xd1: printf("POP   D"); break;
        case 0xd2: printf("JNC   #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0xd3: printf("OUT   #%02X", code[1]); opbytes = 2; break;
        case 0xd4: printf("CNC   #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0xd5: printf("PUSH  D"); break;
        case 0xd6: printf("SUI   #%02X", code[1]); opbytes = 2; break;
        case 0xd7: printf("RST   2"); break;
        case 0xd8: printf("RC"); break;
        case 0xd9: printf("RET"); break;
        case 0xda: printf("JC    #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0xdb: printf("IN    #%02X", code[1]); opbytes = 2; break;
        case 0xdc: printf("CC    #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0xdd: printf("CALL  #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0xde: printf("SBI   #%02X", code[1]); opbytes = 2; break;
        case 0xdf: printf("RST   3"); break;

        case 0xe0: printf("RPO"); break;
        case 0xe1: printf("POP   H"); break;
        case 0xe2: printf("JPO   #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0xe3: printf("XTHL"); break;
        case 0xe4: printf("CPO   #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0xe5: printf("PUSH  H"); break;
        case 0xe6: printf("ANI   #%02X", code[1]); opbytes = 2; break;
        case 0xe7: printf("RST   4"); break;
        case 0xe8: printf("RPE"); break;
        case 0xe9: printf("PCHL"); break;
        case 0xea: printf("JPE    #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0xeb: printf("XCHG"); break;
        case 0xec: printf("CPE    #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0xed: printf("CALL  #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0xee: printf("XRI   #%02X", code[1]); opbytes = 2; break;
        case 0xef: printf("RST   5"); break;

        case 0xf0: printf("RP"); break;
        case 0xf1: printf("POP   PSW"); break;
        case 0xf2: printf("JP   #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0xf3: printf("DI"); break;
        case 0xf4: printf("CP   #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0xf5: printf("PUSH  PSW"); break;
        case 0xf6: printf("ORI   #%02X", code[1]); opbytes = 2; break;
        case 0xf7: printf("RST   6"); break;
        case 0xf8: printf("RM"); break;
        case 0xf9: printf("SPHL"); break;
        case 0xfa: printf("JM    #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0xfb: printf("EI"); break;
        case 0xfc: printf("CM    #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0xfd: printf("CALL  #%02X%02X", code[2], code[1]); opbytes = 3; break;
        case 0xfe: printf("CPI   #%02X", code[1]); opbytes = 2; break;
        case 0xff: printf("RST   7"); break;
    }
    return opbytes;
}

void UnimplementedInstruction (State8080* state) {
    printf("Error: Unimplemented Instruction.\n");
    exit(1);
}
/*
    offsetHelper - Helper function that combines the 2 8 bit registers h (high) and l (low) registers into an address in the 16 bit memory.

    Explanation:
    Uses the bitwise left-shift (<<) operator on h to move the 8 bit value 8 bits to the left, and performs the OR (|) operator to combine it with l;
    e.g h = 1100 1010 
        l = 1111 1001

        Using the left-shift (<<) on h:
        h = 1100 1010 0000 0000

        Performing the OR (|) operation on h with l:
        h = 1100 1010 0000 0000
        l = 0000 0000 1111 1001
        _______________________
            1100 1010 1111 1001 -> Offset address to be returned.
*/
uint16_t inline offsetHelper (State8080 *state) {
    return (state->h << 8) | state->l;     
}

int Emulate8080OpCode(State8080* state) {
    uint8_t *memory = state->memory;
    unsigned char *opcode = &memory[state->pc];

    switch(*opcode) 
    {
        case 0x00:break; //NOP - No Operation
        case 0x01:                                        //LXI - B, word
                  state->c = opcode[1];
                  state->b = opcode[2];
                  state->pc+=2;                           //Advance OpCode by 2                  
                  break; 
        case 0x02:UnimplementedInstruction(state); break;
        case 0x03:UnimplementedInstruction(state); break;
        case 0x04:UnimplementedInstruction(state); break;
        case 0x05:UnimplementedInstruction(state); break;
        case 0x06:UnimplementedInstruction(state); break;
        case 0x07:UnimplementedInstruction(state); break;
        case 0x08:UnimplementedInstruction(state); break;
        case 0x09:UnimplementedInstruction(state); break;
        case 0x0a:UnimplementedInstruction(state); break;
        case 0x0b:UnimplementedInstruction(state); break;
        case 0x0c:UnimplementedInstruction(state); break;
        case 0x0d:UnimplementedInstruction(state); break;
        case 0x0e:UnimplementedInstruction(state); break;
        case 0x0f:UnimplementedInstruction(state); break;
        
        case 0x10:UnimplementedInstruction(state); break;
        case 0x11:UnimplementedInstruction(state); break;
        case 0x12:UnimplementedInstruction(state); break;
        case 0x13:UnimplementedInstruction(state); break;
        case 0x14:UnimplementedInstruction(state); break;
        case 0x15:UnimplementedInstruction(state); break;
        case 0x16:UnimplementedInstruction(state); break;
        case 0x17:UnimplementedInstruction(state); break;
        case 0x18:UnimplementedInstruction(state); break;
        case 0x19:UnimplementedInstruction(state); break;
        case 0x1a:UnimplementedInstruction(state); break;
        case 0x1b:UnimplementedInstruction(state); break;
        case 0x1c:UnimplementedInstruction(state); break;
        case 0x1d:UnimplementedInstruction(state); break;
        case 0x1e:UnimplementedInstruction(state); break;
        case 0x1f:UnimplementedInstruction(state); break;

        case 0x20:UnimplementedInstruction(state); break;
        case 0x21:UnimplementedInstruction(state); break;
        case 0x22:UnimplementedInstruction(state); break;
        case 0x23:UnimplementedInstruction(state); break;
        case 0x24:UnimplementedInstruction(state); break;
        case 0x25:UnimplementedInstruction(state); break;
        case 0x26:UnimplementedInstruction(state); break;
        case 0x27:UnimplementedInstruction(state); break;
        case 0x28:UnimplementedInstruction(state); break;
        case 0x29:UnimplementedInstruction(state); break;
        case 0x2a:UnimplementedInstruction(state); break;
        case 0x2b:UnimplementedInstruction(state); break;
        case 0x2c:UnimplementedInstruction(state); break;
        case 0x2d:UnimplementedInstruction(state); break;
        case 0x2e:UnimplementedInstruction(state); break;
        case 0x2f:UnimplementedInstruction(state); break;
        
        case 0x30:UnimplementedInstruction(state); break;
        case 0x31:UnimplementedInstruction(state); break;
        case 0x32:UnimplementedInstruction(state); break;
        case 0x33:UnimplementedInstruction(state); break;
        case 0x34:UnimplementedInstruction(state); break;
        case 0x35:UnimplementedInstruction(state); break;
        case 0x36:UnimplementedInstruction(state); break;
        case 0x37:UnimplementedInstruction(state); break;
        case 0x38:UnimplementedInstruction(state); break;
        case 0x39:UnimplementedInstruction(state); break;
        case 0x3a:UnimplementedInstruction(state); break;
        case 0x3b:UnimplementedInstruction(state); break;
        case 0x3c:UnimplementedInstruction(state); break;
        case 0x3d:UnimplementedInstruction(state); break;
        case 0x3e:UnimplementedInstruction(state); break;
        case 0x3f:UnimplementedInstruction(state); break;

        case 0x40:state->b = state->b; break;  //MOV B, B
        case 0x41:state->b = state->c; break;  //MOV B, C
        case 0x42:state->b = state->d; break;  //MOV B, D
        case 0x43:state->b = state->e; break;  //MOV B, E
        case 0x44:state->b = state->h; break;  //MOV B, H
        case 0x45:state->b = state->l; break;  //MOV B, L
        case 0x46:state->b = state->memory[offsetHelper(state)]; break;  //MOV B, M 
        case 0x47:state->b = state->a; break;  //MOV B, A
        case 0x48:state->c = state->b; break;  //MOV C, B
        case 0x49:state->c = state->c; break;  //MOV C, C
        case 0x4a:state->c = state->d; break;  //MOV C, D
        case 0x4b:state->c = state->e; break;  //MOV C, E
        case 0x4c:state->c = state->h; break;  //MOV C, H
        case 0x4d:state->c = state->l; break;  //MOV C, L
        case 0x4e:state->c = state->memory[offsetHelper(state)]; break;  //MOV C, M
        case 0x4f:state->c = state->a; break;  //MOV C, A
        
        case 0x50:state->d = state->b; break;  //MOV D, B
        case 0x51:state->d = state->c; break;  //MOV D, C
        case 0x52:state->d = state->d; break;  //MOV D, D
        case 0x53:state->d = state->e; break;  //MOV D, E
        case 0x54:state->d = state->h; break;  //MOV D, H
        case 0x55:state->d = state->l; break;  //MOV D, L
        case 0x56:state->d = state->memory[offsetHelper(state)]; break;  //MOV D, M
        case 0x57:state->d = state->a; break;  //MOV D, A
        case 0x58:state->e = state->b; break;  //MOV E, B
        case 0x59:state->e = state->c; break;  //MOV E, C
        case 0x5a:state->e = state->d; break;  //MOV E, D
        case 0x5b:state->e = state->e; break;  //MOV E, E
        case 0x5c:state->e = state->h; break;  //MOV E, H
        case 0x5d:state->e = state->l; break;  //MOV E, L
        case 0x5e:state->e = state->memory[offsetHelper(state)]; break;  //MOV E, M
        case 0x5f:state->e = state->a; break;  //MOV E, A

        case 0x60:state->h = state->b; break;  //MOV H, B
        case 0x61:state->h = state->c; break;  //MOV H, C
        case 0x62:state->h = state->d; break;  //MOV H, D
        case 0x63:state->h = state->e; break;  //MOV H, E
        case 0x64:state->h = state->h; break;  //MOV H, H
        case 0x65:state->h = state->l; break;  //MOV H, L
        case 0x66:state->h = state->memory[offsetHelper(state)]; break;  //MOV H, M
        case 0x68:state->l = state->b; break;  //MOV L, B
        case 0x69:state->l = state->c; break;  //MOV L, C
        case 0x6a:state->l = state->d; break;  //MOV L, D
        case 0x6b:state->l = state->e; break;  //MOV L, E
        case 0x6c:state->l = state->h; break;  //MOV L, H
        case 0x6d:state->l = state->l; break;  //MOV L, L
        case 0x6e:state->l = state->memory[offsetHelper(state)]; break;  //MOV L, M
        case 0x6f:state->l = state->a; break;  //MOV L, A
        
        case 0x70:state->d = state->b; break;  //MOV M, B //TODO
        case 0x71:state->d = state->c; break;  //MOV M, C //TODO
        case 0x72:state->d = state->d; break;  //MOV M, D //TODO
        case 0x73:state->d = state->e; break;  //MOV M, E //TODO
        case 0x74:state->d = state->h; break;  //MOV M, H //TODO
        case 0x75:state->d = state->l; break;  //MOV M, L //TODO
        case 0x76:UnimplementedInstruction(state); break; // HLT
        case 0x77:state->a = state->a; break;  //MOV M, A //TODO
        case 0x78:state->a = state->b; break;  //MOV A, B
        case 0x79:state->a = state->c; break;  //MOV A, C
        case 0x7a:state->a = state->d; break;  //MOV A, D
        case 0x7b:state->a = state->e; break;  //MOV A, E
        case 0x7c:state->a = state->h; break;  //MOV A, H
        case 0x7d:state->a = state->l; break;  //MOV A, L
        case 0x7e:state->a = state->memory[offsetHelper(state)]; break;  //MOV A, M
        case 0x7f:state->a = state->a; break;  //MOV A, A
        
        case 0x80:UnimplementedInstruction(state); break;
        case 0x81:UnimplementedInstruction(state); break;
        case 0x82:UnimplementedInstruction(state); break;
        case 0x83:UnimplementedInstruction(state); break;
        case 0x84:UnimplementedInstruction(state); break;
        case 0x85:UnimplementedInstruction(state); break;
        case 0x86:UnimplementedInstruction(state); break;
        case 0x87:UnimplementedInstruction(state); break;
        case 0x88:UnimplementedInstruction(state); break;
        case 0x89:UnimplementedInstruction(state); break;
        case 0x8a:UnimplementedInstruction(state); break;
        case 0x8b:UnimplementedInstruction(state); break;
        case 0x8c:UnimplementedInstruction(state); break;
        case 0x8d:UnimplementedInstruction(state); break;
        case 0x8e:UnimplementedInstruction(state); break;
        case 0x8f:UnimplementedInstruction(state); break;

        case 0x90:UnimplementedInstruction(state); break;
        case 0x91:UnimplementedInstruction(state); break;
        case 0x92:UnimplementedInstruction(state); break;
        case 0x93:UnimplementedInstruction(state); break;
        case 0x94:UnimplementedInstruction(state); break;
        case 0x95:UnimplementedInstruction(state); break;
        case 0x96:UnimplementedInstruction(state); break;
        case 0x97:UnimplementedInstruction(state); break;
        case 0x98:UnimplementedInstruction(state); break;
        case 0x99:UnimplementedInstruction(state); break;
        case 0x9a:UnimplementedInstruction(state); break;
        case 0x9b:UnimplementedInstruction(state); break;
        case 0x9c:UnimplementedInstruction(state); break;
        case 0x9d:UnimplementedInstruction(state); break;
        case 0x9e:UnimplementedInstruction(state); break;
        case 0x9f:UnimplementedInstruction(state); break;
        
        case 0xa0:UnimplementedInstruction(state); break;
        case 0xa1:UnimplementedInstruction(state); break;
        case 0xa2:UnimplementedInstruction(state); break;
        case 0xa3:UnimplementedInstruction(state); break;
        case 0xa4:UnimplementedInstruction(state); break;
        case 0xa5:UnimplementedInstruction(state); break;
        case 0xa6:UnimplementedInstruction(state); break;
        case 0xa7:UnimplementedInstruction(state); break;
        case 0xa8:UnimplementedInstruction(state); break;
        case 0xa9:UnimplementedInstruction(state); break;
        case 0xaa:UnimplementedInstruction(state); break;
        case 0xab:UnimplementedInstruction(state); break;
        case 0xac:UnimplementedInstruction(state); break;
        case 0xad:UnimplementedInstruction(state); break;
        case 0xae:UnimplementedInstruction(state); break;
        case 0xaf:UnimplementedInstruction(state); break;

        case 0xb0:UnimplementedInstruction(state); break;
        case 0xb1:UnimplementedInstruction(state); break;
        case 0xb2:UnimplementedInstruction(state); break;
        case 0xb3:UnimplementedInstruction(state); break;
        case 0xb4:UnimplementedInstruction(state); break;
        case 0xb5:UnimplementedInstruction(state); break;
        case 0xb6:UnimplementedInstruction(state); break;
        case 0xb7:UnimplementedInstruction(state); break;
        case 0xb8:UnimplementedInstruction(state); break;
        case 0xb9:UnimplementedInstruction(state); break;
        case 0xba:UnimplementedInstruction(state); break;
        case 0xbb:UnimplementedInstruction(state); break;
        case 0xbc:UnimplementedInstruction(state); break;
        case 0xbd:UnimplementedInstruction(state); break;
        case 0xbe:UnimplementedInstruction(state); break;
        case 0xbf:UnimplementedInstruction(state); break;
        
        case 0xc0:UnimplementedInstruction(state); break;
        case 0xc1:UnimplementedInstruction(state); break;
        case 0xc2:UnimplementedInstruction(state); break;
        case 0xc3:UnimplementedInstruction(state); break;
        case 0xc4:UnimplementedInstruction(state); break;
        case 0xc5:UnimplementedInstruction(state); break;
        case 0xc6:UnimplementedInstruction(state); break;
        case 0xc7:UnimplementedInstruction(state); break;
        case 0xc8:UnimplementedInstruction(state); break;
        case 0xc9:UnimplementedInstruction(state); break;
        case 0xca:UnimplementedInstruction(state); break;
        case 0xcb:UnimplementedInstruction(state); break;
        case 0xcc:UnimplementedInstruction(state); break;
        case 0xcd:UnimplementedInstruction(state); break;
        case 0xce:UnimplementedInstruction(state); break;
        case 0xcf:UnimplementedInstruction(state); break;

        case 0xd0:UnimplementedInstruction(state); break;
        case 0xd1:UnimplementedInstruction(state); break;
        case 0xd2:UnimplementedInstruction(state); break;
        case 0xd3:UnimplementedInstruction(state); break;
        case 0xd4:UnimplementedInstruction(state); break;
        case 0xd5:UnimplementedInstruction(state); break;
        case 0xd6:UnimplementedInstruction(state); break;
        case 0xd7:UnimplementedInstruction(state); break;
        case 0xd8:UnimplementedInstruction(state); break;
        case 0xd9:UnimplementedInstruction(state); break;
        case 0xda:UnimplementedInstruction(state); break;
        case 0xdb:UnimplementedInstruction(state); break;
        case 0xdc:UnimplementedInstruction(state); break;
        case 0xdd:UnimplementedInstruction(state); break;
        case 0xde:UnimplementedInstruction(state); break;
        case 0xdf:UnimplementedInstruction(state); break;
        
        case 0xe0:UnimplementedInstruction(state); break;
        case 0xe1:UnimplementedInstruction(state); break;
        case 0xe2:UnimplementedInstruction(state); break;
        case 0xe3:UnimplementedInstruction(state); break;
        case 0xe4:UnimplementedInstruction(state); break;
        case 0xe5:UnimplementedInstruction(state); break;
        case 0xe6:UnimplementedInstruction(state); break;
        case 0xe7:UnimplementedInstruction(state); break;
        case 0xe8:UnimplementedInstruction(state); break;
        case 0xe9:UnimplementedInstruction(state); break;
        case 0xea:UnimplementedInstruction(state); break;
        case 0xeb:UnimplementedInstruction(state); break;
        case 0xec:UnimplementedInstruction(state); break;
        case 0xed:UnimplementedInstruction(state); break;
        case 0xee:UnimplementedInstruction(state); break;
        case 0xef:UnimplementedInstruction(state); break;

        case 0xf0:UnimplementedInstruction(state); break;
        case 0xf1:UnimplementedInstruction(state); break;
        case 0xf2:UnimplementedInstruction(state); break;
        case 0xf3:UnimplementedInstruction(state); break;
        case 0xf4:UnimplementedInstruction(state); break;
        case 0xf5:UnimplementedInstruction(state); break;
        case 0xf6:UnimplementedInstruction(state); break;
        case 0xf7:UnimplementedInstruction(state); break;
        case 0xf8:UnimplementedInstruction(state); break;
        case 0xf9:UnimplementedInstruction(state); break;
        case 0xfa:UnimplementedInstruction(state); break;
        case 0xfb:UnimplementedInstruction(state); break;
        case 0xfc:UnimplementedInstruction(state); break;
        case 0xfd:UnimplementedInstruction(state); break;
        case 0xfe:UnimplementedInstruction(state); break;
        case 0xff:UnimplementedInstruction(state); break;
    }
    state->pc+=1; //Advances the opcode by 1.


}

int main (int argc, char**argv)
{
    FILE *f = fopen(argv[1], "rb");
    if (f==NULL)
    {
        printf("Error: couldn't open %s\n", argv[1]);
        exit(1);
    }
    
    fseek(f, 0L, SEEK_END);
    int fsize = ftell(f);
    fseek(f, 0L, SEEK_SET);

    unsigned char *buffer = malloc(fsize);

    fread(buffer, fsize, 1, f);
    fclose(f);

    int pc = 0;

    while (pc < fsize) {
        pc += Disassemble8080OpCode(buffer, pc);
        printf("\n");
    }
    return 0;
}