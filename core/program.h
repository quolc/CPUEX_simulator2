#ifndef PROGRAM
#define PROGRAM

#include <vector>
#include <map>
#include "instruction.h"

class Program {
public:
    char* asmfile;
    std::vector<Instruction> instructions;
    std::map<string, int> labels;

    Program(char*);
    bool loadAssembly();
};

#endif
