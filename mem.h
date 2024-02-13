#ifndef _MEM_H_
#define _MEM_H_
#include <iostream>
#include <cstdint>
#include <string>
using namespace std;

// This is the memory interface
// We assume that memory is zero-filled


// Write the given byte value at the given address
extern void mem_write8(uint64_t address, uint8_t data);

extern string opcode(uint64_t a);

extern void fail(uint64_t xD);

extern bool checkFailure();

extern bool opcodeInterpret(string s, uint64_t xD);

extern uint64_t CBNZ(string s, uint64_t PC);

extern void ADRP(string s, uint64_t PC);

extern void ADDI(string s);

extern void MOVZ(string s);

extern void LDRONE(string s);

extern void LDRTWO(string s);

extern void LDRTHREE(string s);

extern void LDRBONE(string s);

extern void LDRBTWO(string s);

extern void LDRBTHREE(string s);

extern void STRBONE(string s);

extern void STRBTWO(string s);

extern void STRBTHREE(string s);

#endif
