#ifndef SIMULATION
#define SIMULATION

#include "program.h"

class Simulation {
public:
    Program* program;

    virtual void init() = 0;
    virtual void run() = 0;
    virtual void finalize() = 0;
};

#endif
