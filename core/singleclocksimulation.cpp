#include <iostream>
#include "program.h"
#include "simulation.h"
#include "singleclocksimulation.h"

#include <sys/time.h>

struct timeval start, end;

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
    cerr << "initializing simulation..." << endl;

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

    cerr << "done." << endl;
}

void SingleClockSimulation::run() {
    cerr << "runnnig simulation..." << endl;

    gettimeofday(&start, NULL);

    while (!isStopped) {
        if (issued_count % 10000000 == 0) {
            cerr << issued_count << endl;
        }

        step();
    }

    gettimeofday(&end, NULL);
    double microseconds = (end.tv_sec - start.tv_sec) * 1000 * 1000 + (end.tv_usec - start.tv_usec);
    double seconds = microseconds / 1000000;
    cerr << microseconds / 1000 << " ms" << endl;
    cerr << (int)(issued_count / seconds) << " instructions/s" << endl;
}

void SingleClockSimulation::finalize() {
    cerr << "total " << (long)executed_count << " instructions executed." << endl;
    cerr << "total " << (long)issued_count << " instructions issueed." << endl;
}

void SingleClockSimulation::step() {
    bool ret = true;;
    bool condition = false;;
    if (pc >= (program->instructions.size())) {
        this->exit = true;
        cerr << "program exited." << endl;
        return;
    }

    Instruction* inst = &(program->instructions[pc]);
    issued_count++;

    pc++;

    switch (inst->condition) {
        case AL:
            condition = true;
        case NV:
            break;
        case EQ:
            if (cz) condition = true;
            break;
        case NE:
            if (!cz) condition = true;
            break;
        case MI:
            if (cn) condition = true;
            break;
        case PL:
            if (!cn) condition = true;
            break;
        case VS:
            if (cv) condition = true;
            break;
        case VC:
            if (!cv) condition = true;
            break;
        case CS:
            if (cc) condition = true;
            break;
        case CC:
            if (!cc) condition = true;
            break;
        case HI:
            if (cc && !cz) condition = true;
            break;
        case LS:
            if (!cc || cz) condition = true;
            break;
        case GE:
            if (cn == cv) condition = true;
            break;
        case LT:
            if (cn != cv) condition = true;
            break;
        case GT:
            if (!cz && (cn == cv)) condition = true;
            break;
        case LE:
            if (cz && (cn != cv)) condition = true;
            break;
    }

    if (condition) {
        executed_count++;
//        ret = (this->*procTable[inst->opcode])(inst);
        switch (inst->opcode) {
            case MOV:
                ret = proc_mov(inst);
                break;
            case MIF:
                ret = proc_mif(inst);
                break;
            case MFI:
                ret = proc_mfi(inst);
                break;
            case LDW:
                ret = proc_ldw(inst);
                break;
            case LDF:
                ret = proc_ldf(inst);
                break;
            case STW:
                ret = proc_stw(inst);
                break;
            case STF:
                ret = proc_stf(inst);
                break;
            case PRT:
                ret = proc_prt(inst);
                break;
            case SCN:
                ret = proc_scn(inst);
                break;
            case JMP:
                ret = proc_jmp(inst);
                break;
            case CAL:
                ret = proc_cal(inst);
                break;
            case MVH:
                ret = proc_mvh(inst);
                break;
            case MVL:
                ret = proc_mvl(inst);
                break;
            case ADD:
                ret = proc_add(inst);
                break;
            case SUB:
                ret = proc_sub(inst);
                break;
            case MUL:
                ret = proc_mul(inst);
                break;
            case SLA:
                ret = proc_sla(inst);
                break;
            case SRA:
                ret = proc_sra(inst);
                break;
            case INV:
                ret = proc_inv(inst);
                break;
            case AND:
                ret = proc_and(inst);
                break;
            case OOR:
                ret = proc_oor(inst);
                break;
            case XOR:
                ret = proc_xor(inst);
                break;
            case SLL:
                ret = proc_sll(inst);
                break;
            case SRL:
                ret = proc_srl(inst);
                break;
            case CTD:
                ret = proc_ctd(inst);
                break;
            case SQR:
                ret = proc_sqr(inst);
                break;
            case NEG:
                ret = proc_neg(inst);
                break;
            case NOP:
                break;
            case HLT:
                ret = proc_hlt(inst);
                break;
        }
        if (!ret) {
            this->error = true;
            cerr << "an error happend at line " << inst->line << endl;
        }
    }

    if (inst->hlt) {
        halted = true;
        cerr << "halted at line " << inst->line << endl;
    }
}

