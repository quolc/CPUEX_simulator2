#ifndef SINGLECLOCKSIMULATION
#define SINGLECLOCKSIMULATION

#include "simulation.h"

#define MEMORY_SIZE (1024*1024)
#define isStopped   (halted || exit || error)

typedef bool (LP_PROC)(Instruction*);

class SingleClockSimulation : public Simulation {
public:
    // simulation fields
    int pc;
    int* r;
    float* f;
    int* ram;
    int ram_size;
    bool cz, cn, cv, cc;

    bool halted, error, exit;

    // meta setting fields
    bool remote;

    // log firelds
    long issued_count;
    long executed_count;

    // functions
    SingleClockSimulation(Program* prog, bool remote, int memsize);
    ~SingleClockSimulation();
    void init();
    void run();
    void finalize();

    void step();
    bool willExecuteNextStep();

    int fetch_r(Opland* o);
    float fetch_f(Opland* o);
    void set_r(Opland* o, int v);
    void set_f(Opland* o, float v);

    // instructions
    bool proc_mov(Instruction*);
    bool proc_mif(Instruction*);
    bool proc_mfi(Instruction*);
    bool proc_ldw(Instruction*);
    bool proc_ldf(Instruction*);
    bool proc_stw(Instruction*);
    bool proc_stf(Instruction*);
    bool proc_prt(Instruction*);
    bool proc_scn(Instruction*);
    bool proc_jmp(Instruction*);
    bool proc_cal(Instruction*);
    bool proc_mvh(Instruction*);
    bool proc_mvl(Instruction*);
    bool proc_add(Instruction*);
    bool proc_sub(Instruction*);
    bool proc_mul(Instruction*);
    bool proc_sla(Instruction*);
    bool proc_sra(Instruction*);
    bool proc_inv(Instruction*);
    bool proc_and(Instruction*);
    bool proc_oor(Instruction*);
    bool proc_xor(Instruction*);
    bool proc_sll(Instruction*);
    bool proc_srl(Instruction*);
    bool proc_ctd(Instruction*);
    bool proc_sqr(Instruction*);
    bool proc_neg(Instruction*);
    bool proc_nop(Instruction*);
    bool proc_hlt(Instruction*);
};

#endif SINGLECLOCKSIMULATION
