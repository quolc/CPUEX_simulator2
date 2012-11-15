#ifndef OPLAND
#define OPLAND

#include <string>

enum OplandType {
    J, R, F, I, A_H, A_L
};

class Opland {
public:
    OplandType type;
    int index;
    int immediate;
    std::string label;

    bool null;

    Opland();
};

#endif OPLAND
