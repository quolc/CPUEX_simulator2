#include <iostream>
#include <fstream>
#include <string>
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
        cerr << str << endl;

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

    // label insertion for hlt pseudo-instruction

    // do macro procsesing

    return true;
}

