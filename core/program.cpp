#include <iostream>
#include <fstream>
#include <string>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include "instruction.h"
#include "program.h"

using namespace std;

Program::Program(char* asmfile) {
    this->asmfile = asmfile;
}

/*
 * return false when loading assembly was failed.
 */
bool Program::loadAssembly() {
    ifstream ifs(this->asmfile);
    string str;

    cerr << "loading " << (this->asmfile) << "..." << endl;

    // load each line of assembly file.
    int l = 0;
    while (getline(ifs, str)) {
        l++;
        cerr << l << "\t" << str << endl;

        // remove comment
        string::size_type comment_start = str.find("#");
        if (comment_start != string::npos) {
            str = str.substr(0, comment_start);
        }

        // trim start/end and skip empty line
        string str2 = str;
        boost::trim(str2);
        if (str2.size() == 0) continue;

        if (isspace(str[0])) { // instruction
            Instruction instruction = parseInstruction(str);
            if (instruction.failed) {
                cerr << "failed to parse instruction at line " << l << endl;
                return false;
            }
            instruction.line = l;
            this->instructions.push_back(instruction);
        } else { // label
            if (str2[str2.size() - 1] != ':') {
                cerr << "invalid label format at line " << l << endl;
                return false;
            }
            string label = str2.substr(0, str2.size()-1);
            map<string, int>::iterator i = this->labels.find(label);
            if (i == this->labels.end()) {
                this->labels[label] = this->instructions.size();
            } else {
                cerr << "duplicated label at line " << l << endl;
                return false;
            }
        }
    }

    // label insertion for pseudo-instructions
    // hlt
    // subi
    // mov
    // nop
    vector<Instruction>::iterator itr;
    for (itr = this->instructions.begin(); itr != this->instructions.end(); itr++) {
        if (itr->opcode == HLT) {
            // generate a label
            string hlt_label = (boost::format("hlt_here_%d") % itr->line).str();
            this->labels[hlt_label] = itr->line;
            // hlt -> jmp
            itr->opcode = JMP;
            itr->oplands[0].type = J;
            itr->oplands[0].label = hlt_label;
            itr->hlt = true;
        } else if (itr->opcode == SUB && !itr->fl && itr->immediate) {
            itr->opcode = ADD;
            itr->oplands[2].immediate = -(itr->oplands[2].immediate);
        } else if (itr->opcode == MOV && !itr->fl) {
            itr->opcode = ADD;
            itr->immediate = true;
            itr->oplands[2].type = I;
            itr->oplands[2].immediate = 0;
        } else if (itr->opcode == NOP) {
            itr->opcode = ADD;
            itr->immediate = true;
            itr->oplands[0].type = R;
            itr->oplands[1].type = R;
            itr->oplands[2].type = I;
            itr->oplands[0].index = 0;
            itr->oplands[1].index = 0;
            itr->oplands[2].immediate = 0;
        }
    }

    // h16(), l16() macro processing
    for (itr = this->instructions.begin(); itr != this->instructions.end(); itr++) {
        for (int i=0; i<MAXIMUM_OPLAND; i++) {
            Opland* opland = &(itr->oplands[i]);
            int addr = -1;
            if (opland->type == A_H || opland->type == A_L) {
                map<string, int>::iterator i = this->labels.find(opland->label);
                if (i == this->labels.end()) {
                    cerr << "(macro processing) missing label " << opland->label << " at line " << (itr->line) << endl;
                    return false;
                } else {
                    addr = labels[opland->label];
                }
            }
            if (opland->type == A_H) {
                opland->type = I;
                opland->immediate = addr >> 16;
            } else if (opland->type == A_L) {
                opland->type = I;
                opland->immediate = addr & 65535;
            }
        }
    }

    // jump label validation
    for (itr = this->instructions.begin(); itr != this->instructions.end(); itr++) {
        for (int i=0; i<MAXIMUM_OPLAND; i++) {
            Opland* opland = &(itr->oplands[i]);
            if (opland->type == J) {
                map<string, int>::iterator li = this->labels.find(opland->label);
                if (li == this->labels.end()) {
                    cerr << "(jump label validation) missing label " << opland->label << " at line " << (itr->line) << endl;
                    return false;
                }
            }
        }
    }

    if (l==0) return false;

    return true;
}

