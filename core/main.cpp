#include <iostream>
#include <boost/program_options.hpp>

#include "program.h"
#include "simulation.h"
#include "singleclocksimulation.h"

using namespace std;
using namespace boost::program_options;

/*
 * Basic Usage
 * $ ./core hoge.s
 * $ ./core hoge.s < input > output
 *
 * Remote Debugging
 * $ ./core hoge.s -r 8080  ... setup the server and wait for a connection of clients.
 *
 * Simulation Options
 * -S       ... diable cycle acculate mode (fast)
 * -M 1024  ... set the memory size (W).
 */
int main(int argc, char* argv[]) {
    cerr << "CPU Simulator for IIDX tricoro's 2nd Architecture.\n" << endl;

    // Command Line Arguments Processing
    options_description options("Options");

    int port = 0;
    bool remote = false;
    bool single = false;
    int memory = 0;

    options.add_options()
        ("remote,r", value<int>(&port), "Remote Debug Mode")
        ("single,s", "Single Clock Mode")
        ("memory,M", value<int>(&memory), "Memory Size [Word]")
        ("help,h", "Help");

    variables_map vm;

    try {
        store(parse_command_line(argc, argv, options), vm);
        notify(vm);

        if (argc < 2) {
            cerr << "!! Please Specify the Assembly File Name as the First Argument. !!\n" << endl;
            return 0;
        }
        if (vm.count("help")) {
            cerr << options << endl;
            return 0;
        }

        if (vm.count("remote"))
            remote = true;
        if (vm.count("single"))
            single = true;
    } catch(exception &e) {
        cerr << options << endl;
        cerr << e.what() << endl;
    }

    if (remote)
        cerr << "-- Remote Debugging Mode (port " << port << ")" << endl;
    if (single)
        cerr << "-- Single Clock Mode" << endl;

    // Parse Assembly File
    Program program(argv[1]);
    if (!program.loadAssembly()) {
        cerr << "failed to load assembly." << endl;
        return 0;
    }

    // Initialize Simulation
    Simulation* s;
    if (single) {
        s = new SingleClockSimulation(&program, remote, memory);
    } else {
        // TODO: MultiClockSimulation
    }

    s->init();
    s->run();
    s->finalize();

    delete(s);

    return 0;
}

