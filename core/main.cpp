#include <iostream>
#include <boost/format.hpp>

using namespace std;

/*
 * Basic Usage
 * $ ./core hoge.s
 * $ ./core hoge.s < input > output
 *
 * Remote Debugging
 * $ ./core hoge.s -r 8080  ... setup the server and wait for a connection of clients.
 *
 * Special Modes
 * $ ./core hoge.s -c       ... only check the validity of the assembly
 *
 * Simulation Options
 * -S       ... diable cycle acculate mode (fast)
 * -M 1024  ... set the memory size (W).
 */
int main(int argc, char* argv[]) {
    cout << "CPU Simulator for CPUEX IIDX tricoro's 2nd Architecture." << endl;

    for (int i=0; i<argc; i++) {
        cout << argv[i] << endl;
    }

    return 0;
}
