#include <iostream>
#include "program.h"
#include "simulation.h"
#include "singleclocksimulation.h"

bool (SingleClockSimulation::*procTable[])(Instruction*) = {
    &SingleClockSimulation::proc_mov,
    &SingleClockSimulation::proc_mif,
    &SingleClockSimulation::proc_mfi,
    &SingleClockSimulation::proc_ldw,
    &SingleClockSimulation::proc_ldf,
    &SingleClockSimulation::proc_stw,
    &SingleClockSimulation::proc_stf,
    &SingleClockSimulation::proc_prt,
    &SingleClockSimulation::proc_scn,
    &SingleClockSimulation::proc_jmp,
    &SingleClockSimulation::proc_cal,
    &SingleClockSimulation::proc_mvh,
    &SingleClockSimulation::proc_mvl,
    &SingleClockSimulation::proc_add,
    &SingleClockSimulation::proc_sub,
    &SingleClockSimulation::proc_mul,
    &SingleClockSimulation::proc_sla,
    &SingleClockSimulation::proc_sra,
    &SingleClockSimulation::proc_inv,
    &SingleClockSimulation::proc_and,
    &SingleClockSimulation::proc_oor,
    &SingleClockSimulation::proc_xor,
    &SingleClockSimulation::proc_sll,
    &SingleClockSimulation::proc_srl,
    &SingleClockSimulation::proc_ctd,
    &SingleClockSimulation::proc_sqr,
    &SingleClockSimulation::proc_neg,
    &SingleClockSimulation::proc_nop,
    &SingleClockSimulation::proc_hlt
};

SingleClockSimulation::SingleClockSimulation(Program* prog, bool remote, int memsize) {
    this->program = prog;

    this->remote = remote;

    r = new int[64];
    f = new float[64];

    ram_size = MEMORY_SIZE;
    if (memsize != 0) {
        ram_size = memsize;
    }
    ram = new int[ram_size];

    halted = false;
    error = false;
    exit = false;
}

SingleClockSimulation::~SingleClockSimulation() {
    delete[] r;
    delete[] f;
    delete[] ram;
}

void SingleClockSimulation::init() {
    cerr << "Initializing Simulation..." << endl;

    pc = 0;

    for (int i=0; i<64; i++) {
        r[i] = 0;
        f[i] = 0;
    }
    cz = false; cn = false; cv = false; cc = false;

    for (int i=0; i<ram_size; i++) {
        ram[i] = 0;
    }

    halted = false; error = false; exit = false;

    issued_count = 0;
    executed_count = 0;
}

void SingleClockSimulation::run() {
    while (!isStopped) {
        step();
    }
}

void SingleClockSimulation::finalize() {
}

bool SingleClockSimulation::willExecuteNextStep() {
    Instruction* instruction = &(this->program->instructions[pc]);

    switch (instruction->condition) {
        case AL:
            return true;
        case NV:
            break;
        case EQ:
            if (cz) return true;
            break;
        case NE:
            if (!cz) return true;
            break;
        case MI:
            if (cn) return true;
            break;
        case PL:
            if (!cn) return true;
            break;
        case VS:
            if (cv) return true;
            break;
        case VC:
            if (!cv) return true;
            break;
        case CS:
            if (cc) return true;
            break;
        case CC:
            if (!cc) return true;
            break;
        case HI:
            if (cc && !cz) return true;
            break;
        case LS:
            if (!cc || cz) return true;
            break;
        case GE:
            if (cn == cv) return true;
            break;
        case LT:
            if (cn != cv) return true;
            break;
        case GT:
            if (!cz && (cn == cv)) return true;
            break;
        case LE:
            if (cz && (cn != cv)) return true;
            break;
    }

    return false;
}

void SingleClockSimulation::step() {
    if (pc >= (int)(program->instructions.size()) || pc < 0) {
        this->exit = true;
        cerr << "program exited." << endl;
        return;
    }

    Instruction* inst = &(program->instructions[pc]);
    pc++;
    issued_count++;

    if (willExecuteNextStep()) {
        executed_count++;
        bool ret = true;
        ret = (this->*procTable[inst->opcode])(inst);
        if (!ret) {
            this->error = true;
            cerr << "an error happend at line " << inst ->line << endl;
        }
    }

    if (inst->hlt) {
        halted = true;
        cerr << "halted at line " << inst->line << endl;
    }
}

int SingleClockSimulation::fetch_r(Opland* o) {
    return this.r[o->index];
}

float SingleClockSimulation::fetch_r(Opland* o) {
    return this.f[o->index];
}

void SingleClockSimulation::fetch_r(Opland* o, int v) {
    if (o->index > 0) {
        r[o->index] = v;
    }
    if (o->index >= 32) {
        f[o->index] = v;
    }
}

void SingleClockSimulation::fetch_r(Opland* o, float v) {
    if (o->index > 0) {
        f[o->index] = v;
    }
    if (o->index >= 32) {
        r[o->index] = v;
    }
}

bool SingleClockSimulation::proc_mov(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_mif(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_mfi(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_ldw(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_ldf(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_stw(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_stf(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_prt(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_scn(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_jmp(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_cal(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_mvh(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_mvl(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_add(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_sub(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_mul(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_sla(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_sra(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_inv(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_and(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_oor(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_xor(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_sll(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_srl(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_ctd(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_sqr(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_neg(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_nop(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_hlt(Instruction* inst) {
    return true;
}

