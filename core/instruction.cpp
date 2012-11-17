#include <string>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include "opland.h"
#include "instruction.h"

using namespace std;

typedef pair<OpCode, string> opcstr;
pair<OpCode, string> opcodes[] = {
    opcstr(MOV, "mov"), opcstr(MIF, "mif"), opcstr(MFI, "mfi"),
    opcstr(LDW, "ldw"), opcstr(LDF, "ldf"), opcstr(STW, "stw"), opcstr(STF, "stf"),
    opcstr(PRT, "prt"), opcstr(SCN, "scn"), opcstr(JMP, "jmp"), opcstr(CAL, "cal"), opcstr(MVH, "mvh"), opcstr(MVL, "mvl"),
    opcstr(ADD, "add"), opcstr(SUB, "sub"), opcstr(MUL, "mul"), opcstr(SLA, "sla"), opcstr(SRA, "sra"), opcstr(INV, "inv"),
    opcstr(AND, "and"), opcstr(OOR, "oor"), opcstr(XOR, "xor"), opcstr(SLL, "sll"), opcstr(SRL, "srl"), opcstr(CTD, "ctd"), opcstr(SQR, "sqr"), opcstr(NEG, "neg"),
    opcstr(NOP, "nop"), opcstr(HLT, "hlt")
};
map<OpCode, string> opcode_string_map;

typedef pair<Condition, string> condstr;
pair<Condition, string> conditions[] = {
    condstr(AL, "AL"), condstr(NV, "NV"), condstr(EQ, "EQ"), condstr(NE, "NE"), condstr(MI, "MI"), condstr(PL, "PL"), condstr(VS, "VS"), condstr(VC, "VC"),
    condstr(CS, "CS"), condstr(CC, "CC"), condstr(HI, "HI"), condstr(LS, "LS"), condstr(GE, "GE"), condstr(LT, "LT"), condstr(GT, "GT"), condstr(LE, "LE")
};
map<Condition, string> condition_string_map;

Instruction::Instruction() {
    this->failed = false;
    this->hlt = false;
}

Instruction parseInstruction(string str) {
    Instruction instruction;

    // initialize only first time
    if (opcode_string_map.size() == 0) {
        opcode_string_map = map<OpCode, string>(opcodes, opcodes + sizeof(opcodes) / sizeof(pair<OpCode, string>));
    }
    if (condition_string_map.size() == 0) {
        condition_string_map = map<Condition, string>(conditions, conditions + sizeof(conditions) / sizeof(pair<Condition, string>));
    }

    boost::regex r("[^,\\s]+");
    boost::smatch m;

    string::const_iterator start=str.begin();
    string::const_iterator end=str.end();

    // opcode
    if (boost::regex_search(start, end, m, r)) {
        string operation = m.str();

        // tag detection
        boost::regex rtag("\\[.*\\]");
        boost::smatch mtag;
        if (boost::regex_search(operation, mtag, rtag)) {
            instruction.tag = mtag.str().substr(1, mtag.str().size()-2);
            operation = operation.substr(0, mtag.position());
        }

        int p = 0;

        // float
        if (operation[p] == 'f') {
            instruction.fl = true;
            p++;
        } else {
            instruction.fl = false;
        }

        // opcode
        if ((int)operation.size() < p+3) {
            instruction.failed = true;
            return instruction;
        }
        string opcode = operation.substr(p, p+3);
        map<OpCode, string>::iterator itr;
        for (itr = opcode_string_map.begin(); itr != opcode_string_map.end(); itr++) {
            if (itr->second == opcode) {
                instruction.opcode = itr->first;
                break;
            }
        }
        p+=3;

        // immediate
        if ((int)operation.size() > p && operation[p] == 'i') {
            instruction.immediate = true;
            p++;
        } else {
            instruction.immediate = false;
        }

        // implicit immediate instructions
        switch (instruction.opcode) {
            case STW:
            case STF:
            case LDW:
            case LDF:
            case PRT:
            case SCN:
                instruction.immediate = true;
                break;
            default:
                break;
        }

        // condition flags
        if ((int)operation.size() > p+1) {
            string condstr = operation.substr(p, p+2);
            map<Condition, string>::iterator conditr;
            for (conditr = condition_string_map.begin(); conditr != condition_string_map.end(); conditr++) {
                if (boost::iequals(conditr->second, condstr)) {
                    instruction.condition = conditr->first;
                    p+=2;
                    break;
                }
            }
            if (conditr == condition_string_map.end()) {
                instruction.failed = true;
                return instruction;
            }
        }

        // condition set flags
        if ((int)operation.size() > p && operation[p] == 's') {
            instruction.conditionset = true;
            p++;
        } else {
            instruction.conditionset = false;
        }

        if ((int)operation.size() != p) {
            instruction.failed = true;
            return instruction;
        }
    }

    // oplands
    for (int i=0; i<MAXIMUM_OPLAND; i++) {
        start = m[0].second;
        if (boost::regex_search(start, end, m, r)) {
            if ((instruction.opcode == JMP || instruction.opcode == CAL) && instruction.immediate) {
                instruction.oplands[i].type = J;
                instruction.oplands[i].label = m.str();
            } else {
                switch (m.str()[0]) {
                    case 'r':
                        instruction.oplands[i].type = R;
                        instruction.oplands[i].index = boost::lexical_cast<int>(m.str().substr(1));
                        break;
                    case 'f':
                        instruction.oplands[i].type = F;
                        instruction.oplands[i].index = boost::lexical_cast<int>(m.str().substr(1));
                        break;
                    case 'h':
                        instruction.oplands[i].type = A_H;
                        instruction.oplands[i].label = m.str().substr(4, m.str().size() - 5);
                        break;
                    case 'l':
                        instruction.oplands[i].type = A_L;
                        instruction.oplands[i].label = m.str().substr(4, m.str().size() - 5);
                        break;
                    default:
                        instruction.oplands[i].type = I;
                        instruction.oplands[i].immediate = boost::lexical_cast<int>(m.str());
                        break;
                }
            }
        } else {
            instruction.oplands[i].null = true;
        }
    }

    // jmp/cal : 1st opland is Rs
    if ((instruction.opcode == JMP || instruction.opcode == CAL) && !instruction.immediate) {
        instruction.oplands[1] = instruction.oplands[0];
        instruction.oplands[0].null = true;
    }

    // prt : 1st opland is Rs, 2nd opland is immediate
    if (instruction.opcode == PRT) {
        instruction.oplands[2] = instruction.oplands[1];
        instruction.oplands[1] = instruction.oplands[0];
        instruction.oplands[0].null = true;
    }

    instruction.raw = str;

    return instruction;
}
