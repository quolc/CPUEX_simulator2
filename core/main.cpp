#include <iostream>
#include <boost/program_options.hpp>

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
    cout << "CPU Simulator for CPUEX IIDX tricoro's 2nd Architecture.\n" << endl;

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
            cout << "!! Please Specify the Assembly File Name as the First Argument. !!\n" << endl;
            return 0;
        }
        if (vm.count("help")) {
            cout << options << endl;
            return 0;
        }

        if (vm.count("remote"))
            remote = true;
        if (vm.count("single"))
            single = true;
    } catch(exception &e) {
        cout << options << endl;
        cout << e.what() << endl;
    }

    cout << "Assembly File: " << argv[1] << endl;
    if (remote)
        cout << "-- Remote Debugging Mode (port " << port << ")" << endl;
    if (single)
        cout << "-- Single Clock Mode" << endl;

    return 0;
}
