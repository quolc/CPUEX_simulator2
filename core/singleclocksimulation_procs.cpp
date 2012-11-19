#include <iostream>
#include <cstdio>
#include <cmath>
#include "fpu.h"
#include "singleclocksimulation.h"

#define fetch_r(o) (r[o.index])
#define fetch_f(o) (f[o.index])
#define set_r(o, v) r[o.index] = v; if (o.index >= 32) f[o.index] = v;
#define set_f(o, v) f[o.index] = v; if (o.index >= 32) r[o.index] = v;

using namespace std;

float fa, fb, fc;
int a, b, c;
long a2, b2;
map<string, int>::iterator itr;

union float32 {
    float fvalue;
    int ivalue;
} f32;

int float2int(float f) {
    f32.fvalue = f;
    return f32.ivalue;
}

float int2float(int i) {
    f32.ivalue = i;
    return f32.fvalue;
}

bool SingleClockSimulation::proc_mov(Instruction* inst) {
    if (inst->fl) {
        fa = fetch_f(inst->oplands[1]);
        set_f(inst->oplands[0], fa);
    } else {
        return false;
    }
    return true;
}

bool SingleClockSimulation::proc_mif(Instruction* inst) {
    if (inst->fl) {
        return false;
    } else {
        a = fetch_r(inst->oplands[1]);
        set_f(inst->oplands[0], int2float(a));
    }
    return true;
}

bool SingleClockSimulation::proc_mfi(Instruction* inst) {
    if (inst->fl) {
        return false;
    } else {
        fa = fetch_f(inst->oplands[1]);
        set_r(inst->oplands[0], float2int(fa));
    }
    return true;
}

bool SingleClockSimulation::proc_ldw(Instruction* inst) {
    if (inst->fl) {
        return false;
    } else {
        a = fetch_r(inst->oplands[1]);
        b = inst->oplands[2].immediate;
        a += b;

        if (a < 0 || ram_size <= a) {
            return false;
        }
        c = ram[a];
        set_r(inst->oplands[0], c);
    }
    return true;
}

bool SingleClockSimulation::proc_ldf(Instruction* inst) {
    if (inst->fl) {
        return false;
    } else {
        a = fetch_r(inst->oplands[1]);
        b = inst->oplands[2].immediate;
        a += b;

        if (a < 0 || ram_size <= a) {
            return false;
        }
        c = ram[a];
        set_f(inst->oplands[0], int2float(c));
    }
    return true;
}

bool SingleClockSimulation::proc_stw(Instruction* inst) {
    if (inst->fl) {
        return false;
    } else {
        a = fetch_r(inst->oplands[1]);
        b = inst->oplands[2].immediate;
        a += b;

        if (a < 0 || ram_size <= a) {
            return false;
        }
        ram[a] = fetch_r(inst->oplands[0]);
    }
    return true;
}

bool SingleClockSimulation::proc_stf(Instruction* inst) {
    if (inst->fl) {
        return false;
    } else {
        a = fetch_r(inst->oplands[1]);
        b = inst->oplands[2].immediate;
        a += b;

        if (a < 0 || ram_size <= a) {
            return false;
        }
        ram[a] = float2int(fetch_f(inst->oplands[0]));
    }
    return true;
}

bool SingleClockSimulation::proc_prt(Instruction* inst) {
    if (inst->fl) {
        a = float2int(fetch_f(inst->oplands[1]));
    } else {
        a = fetch_r(inst->oplands[1]);
    }

    int output = 0;
    int active = inst->oplands[2].immediate;
    if ((active & 8) > 0) output = ((a >> 24) & 255);
    if ((active & 4) > 0) output = ((a >> 16) & 255);
    if ((active & 2) > 0) output = ((a >> 8) & 255);
    if ((active & 1) > 0) output = ((a >> 0) & 255);

    cout << (char)output;

    return true;
}

bool SingleClockSimulation::proc_scn(Instruction* inst) {
    if (inst->fl) {
        return false;
    } else {
        a = getchar();

        b = fetch_r(inst->oplands[1]);
        int active = inst->oplands[2].immediate;
        if ((active & 8) > 0) b = (b & 0x00FFFFFF) + (a << 24);
        if ((active & 4) > 0) b = (b & 0xFF00FFFF) + (a << 16);
        if ((active & 2) > 0) b = (b & 0xFFFF00FF) + (a << 8);
        if ((active & 1) > 0) b = (b & 0xFFFFFF00) + (a << 0);

        set_r(inst->oplands[0], b);
    }
    return true;
}

bool SingleClockSimulation::proc_jmp(Instruction* inst) {
    if (inst->fl) {
        return false;
    } else {
        if (inst->immediate) {
            itr = this->program->labels.find(inst->oplands[0].label);
            pc = itr->second;
        } else {
            pc = fetch_r(inst->oplands[1]);
        }
    }
    return true;
}

bool SingleClockSimulation::proc_cal(Instruction* inst) {
    if (inst->fl) {
        return false;
    } else {
        if (inst->immediate) {
            itr = this->program->labels.find(inst->oplands[0].label);
            r[31] = pc;
            pc = itr->second;
        } else {
            r[31] = pc;
            pc = fetch_r(inst->oplands[1]);
        }
    }
    return true;
}

bool SingleClockSimulation::proc_mvh(Instruction* inst) {
    if (inst->fl) {
        a = float2int(fetch_f(inst->oplands[1]));
        b = inst->oplands[2].immediate;
        c = (a & 65535) | (b << 16);
        set_f(inst->oplands[0], int2float(c));
    } else {
        a = fetch_r(inst->oplands[1]);
        b = inst->oplands[2].immediate;
        c = (a & 65535) | (b << 16);
        set_r(inst->oplands[0], c);
    }
    return true;
}

bool SingleClockSimulation::proc_mvl(Instruction* inst) {
    if (inst->fl) {
        a = float2int(fetch_f(inst->oplands[1]));
        b = inst->oplands[2].immediate;
        c = (a & ~65535) | (b);
        set_f(inst->oplands[0], int2float(c));
    } else {
        a = fetch_r(inst->oplands[1]);
        b = inst->oplands[2].immediate;
        c = (a & ~65535) | (b);
        set_r(inst->oplands[0], c);
    }
    return true;
}

bool SingleClockSimulation::proc_add(Instruction* inst) {
    if (inst->fl) {
        fa = fetch_f(inst->oplands[1]);
        fb = fetch_f(inst->oplands[2]);

        fc = fadd(fa, fb);
        set_f(inst->oplands[0], fc);

        if (inst->conditionset) {
            cz = (fc == 0);
            cn = (fc < 0);
            cv = (fpclassify(fc) == FP_INFINITE);
            cc = false;
        }
    } else {
        a = fetch_r(inst->oplands[1]);
        if (inst->immediate)
            b = inst->oplands[2].immediate;
        else
            b = fetch_r(inst->oplands[2]);
        c = a + b;
        set_r(inst->oplands[0], c);

        if (inst->conditionset) {
            cz = (c == 0);
            cn = (c < 0);
            cv = ((a > 0) && (b > 0) && (c < 0)) || ((a < 0) && (b < 0) && (c > 0));
            a2 = (a > 0) ? a : (1L << 32) + a;
            b2 = (b > 0) ? b : (1L << 32) + b;
            cc = ((a2+b2) >> 32) == 1;
        }
    }
    return true;
}

bool SingleClockSimulation::proc_sub(Instruction* inst) {
    if (inst->fl) {
        fa = fetch_f(inst->oplands[1]);
        fb = fetch_f(inst->oplands[2]);

        fc = fsub(fa, fb);
        set_f(inst->oplands[0], fc);

        if (inst->conditionset) {
            cz = (fc == 0);
            cn = (fc < 0);
            cv = (fpclassify(fc) == FP_INFINITE);
            cc = false;
        }
    } else {
        a = fetch_r(inst->oplands[1]);
        if (inst->immediate)
            b = inst->oplands[2].immediate;
        else
            b = fetch_r(inst->oplands[2]);
        c = a - b;
        set_r(inst->oplands[0], c);

        if (inst->conditionset) {
            cz = (c == 0);
            cn = (c < 0);
            cv = ((a > 0) && (b < 0) && (c < 0)) || ((a < 0) && (b > 0) && (c > 0));
            a2 = (a > 0) ? a : (1L << 32) + a;
            b2 = (b > 0) ? b : (1L << 32) + b;
            cc = ((a2+b2) >> 32) == 1;
        }
    }
    return true;
}

bool SingleClockSimulation::proc_mul(Instruction* inst) {
    if (inst->fl) {
        fa = fetch_f(inst->oplands[1]);
        fb = fetch_f(inst->oplands[2]);

        fc = fmul(fa, fb);
        set_f(inst->oplands[0], fc);

        if (inst->conditionset) {
            cz = (fc == 0);
            cn = (fc < 0);
            cv = (fpclassify(fc) == FP_INFINITE);
            cc = false;
        }
    } else {
        return false;
    }
    return true;
}

bool SingleClockSimulation::proc_sla(Instruction* inst) {
    if (inst->fl) {
        return false;
    } else {
        a = fetch_r(inst->oplands[1]);
        if (inst->immediate)
            b = inst->oplands[2].immediate;
        else
            b = fetch_r(inst->oplands[2]);
        c = a << b;
        set_r(inst->oplands[0], c);

        if (inst->conditionset) {
            cz = (c == 0);
            cn = (c < 0);
            cv = false;
            cc = ((a >> (32-b)) & 1) == 1;
        }
    }
    return true;
}

bool SingleClockSimulation::proc_sra(Instruction* inst) {
    if (inst->fl) {
        return false;
    } else {
        a = fetch_r(inst->oplands[1]);
        if (inst->immediate)
            b = inst->oplands[2].immediate;
        else
            b = fetch_r(inst->oplands[2]);
        c = a >> b;
        c = (c & 0x7FFFFFFF) | (a & 0x80000000);
        set_r(inst->oplands[0], c);

        if (inst->conditionset) {
            cz = (c == 0);
            cn = (c < 0);
            cv = false;
            cc = ((a >> (b-1)) & 1) == 1;
        }
    }
    return true;
}

bool SingleClockSimulation::proc_inv(Instruction* inst) {
    if (inst->fl) {
        fa = fetch_f(inst->oplands[1]);
        fc = finv(fa);
        set_f(inst->oplands[0], fc);
        if (inst->conditionset) {
            cz = (fc == 0);
            cn = (fc < 0);
            cv = (fpclassify(fc) == FP_INFINITE);
            cc = false;
        }
    } else {
        return false;
    }
    return true;
}

bool SingleClockSimulation::proc_and(Instruction* inst) {
    if (inst->fl) {
        return false;
    } else {
        a = fetch_r(inst->oplands[1]);
        if (inst->immediate)
            b = inst->oplands[2].immediate;
        else
            b = fetch_r(inst->oplands[2]);
        c = a & b;
        set_r(inst->oplands[0], c);
        if (inst->conditionset) {
            cz = (c == 0);
            cn = (c < 0);
            cv = false;
            cc = false;
        }
    }
    return true;
}

bool SingleClockSimulation::proc_oor(Instruction* inst) {
    if (inst->fl) {
        return false;
    } else {
        a = fetch_r(inst->oplands[1]);
        if (inst->immediate)
            b = inst->oplands[2].immediate;
        else
            b = fetch_r(inst->oplands[2]);
        c = a | b;
        set_r(inst->oplands[0], c);
        if (inst->conditionset) {
            cz = (c == 0);
            cn = (c < 0);
            cv = false;
            cc = false;
        }
    }
    return true;
}

bool SingleClockSimulation::proc_xor(Instruction* inst) {
    if (inst->fl) {
        return false;
    } else {
        a = fetch_r(inst->oplands[1]);
        if (inst->immediate)
            b = inst->oplands[2].immediate;
        else
            b = fetch_r(inst->oplands[2]);
        c = a & b;
        set_r(inst->oplands[0], c);
        if (inst->conditionset) {
            cz = (c == 0);
            cn = (c < 0);
            cv = false;
            cc = false;
        }
    }
    return true;
}

bool SingleClockSimulation::proc_sll(Instruction* inst) {
    if (inst->fl) {
        return false;
    } else {
        a = fetch_r(inst->oplands[1]);
        if (inst->immediate)
            b = inst->oplands[2].immediate;
        else
            b = fetch_r(inst->oplands[2]);
        c = a << b;
        set_r(inst->oplands[0], c);
        if (inst->conditionset) {
            cz = (c == 0);
            cn = (c < 0);
            cv = false;
            cc = ((a >> (32-b)) & 1) == 1;
        }
    }
    return true;
}

bool SingleClockSimulation::proc_srl(Instruction* inst) {
    if (inst->fl) {
        return false;
    } else {
        a = fetch_r(inst->oplands[1]);
        if (inst->immediate)
            b = inst->oplands[2].immediate;
        else
            b = fetch_r(inst->oplands[2]);
        c = (a >> b) & 0x7FFFFFFF;
        set_r(inst->oplands[0], c);
        if (inst->conditionset) {
            cz = (c == 0);
            cn = (c < 0);
            cv = false;
            cc = ((a >> (b-1)) & 1) == 1;
        }
    }
    return true;
}

bool SingleClockSimulation::proc_ctd(Instruction* inst) {
    if (inst->fl) {
        return false;
    } else {
        a = fetch_r(inst->oplands[1]);

        int hun = a / 100;
        int ten = (a % 100) / 10;
        int one = a % 10;

        set_r(inst->oplands[0], ((hun + (int)'0') << 16) + ((ten + (int)'0') << 8) + ((one + (int)'0')));

        if (inst->conditionset) {
            cv = a < 100;
            cc = a < 10;
        }
    }
    return true;
}

bool SingleClockSimulation::proc_sqr(Instruction* inst) {
    if (inst->fl) {
        a = fetch_f(inst->oplands[1]);
        c = fsqr(a);
        set_f(inst->oplands[0], c);

        if (inst->conditionset) {
            cz = (fc == 0);
            cn = (fc < 0);
            cv = (fpclassify(fc) == FP_INFINITE);
            cc = false;
        }
    } else {
        return false;
    }
    return true;
}

bool SingleClockSimulation::proc_neg(Instruction* inst) {
    if (inst->fl) {
        a = fetch_f(inst->oplands[1]);
        c = fneg(a);
        set_f(inst->oplands[0], c);

        if (inst->conditionset) {
            cz = (fc == 0);
            cn = (fc < 0);
            cv = (fpclassify(fc) == FP_INFINITE);
            cc = false;
        }
    } else {
        return false;
    }
    return true;
}

bool SingleClockSimulation::proc_nop(Instruction* inst) {
    return true;
}

bool SingleClockSimulation::proc_hlt(Instruction* inst) {
    halted = true;
    return true;
}

