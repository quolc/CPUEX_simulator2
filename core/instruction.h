#ifndef INSTRUCTION
#define INSTRUCTION

#include <string>
#include <map>
#include "opland.h"

using namespace std;

#define MAXIMUM_OPLAND 3

enum OpCode {
    MOV, MIF, MFI, LDW, LDF, STW, STF,  // 00xxx
    PRT, SCN, JMP, CAL, MVH, MVL,       // 01xxx
    ADD, SUB, MUL, SLA, SRA, INV,       // 10xxx
    AND, OOR, XOR, SLL, SRL, CTD, SQR, NEG, // 11xxx
    NOP, HLT // 擬似命令
};

enum Condition {
    AL, NV, EQ, NE, MI, PL, VS, VC,
    CS, CC, HI, LS, GE, LT, GT, LE
};

class Instruction {
public:
    // instruction properties
    OpCode opcode;
    Condition condition;
    bool fl;
    bool immediate;
    bool conditionset;
    string raw;

    Opland oplands[MAXIMUM_OPLAND];

    // meta parameters
    string tag;
    bool failed;

    // methods
    Instruction();
};

Instruction parseInstruction(string);

#endif
