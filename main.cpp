#include <iostream>
#include <cstdlib>
#include <string>

#include "proxy_server.h"

using namespace std;

void usage(const char* progname){
    cout << "Usage: " << progname << " [OPTIONS]\n\n";
    cout << "Simple HTTP chaos proxy\n\n";
    cout << "Options:\n";
    cout << "  -b <host>       Backend server IP/hostname\n";
    cout << "  -p <port>       Backend server port\n";
    cout << "  -c <file>       Path to JSON config file\n";
    cout << "  -h              Show this help message\n\n";
    cout << "Examples:\n";
    cout << "  " << progname << " -b 127.0.0.1 -p 5051\n";
    cout << "  " << progname << " -c config.json\n\n";
    cout << "Notes:\n";
    cout << "  - Proxy listens internally on port 5050\n";
    cout << "  - In Docker, expose/map ports externally as needed\n";
}

int main(int argc, char* argv[]){
    string backendHost = "127.0.0.1";
    int backendPort = 5051;
    string configPath;
    bool useConfig = false;

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];

        if (arg == "-h") {
            usage(argv[0]);
            return 0;
        }
        else if (arg == "-c") {
            if (i + 1 >= argc) {
                cerr << "Error: -c requires a file path\n";
                return 1;
            }
            configPath = argv[++i];
            useConfig = true;
        }
        else if (arg == "-b") {
            if (i + 1 >= argc) {
                cerr << "Error: -b requires a backend host\n";
                return 1;
            }
            backendHost = argv[++i];
        }
        else if (arg == "-p") {
            if (i + 1 >= argc) {
                cerr << "Error: -p requires a backend port\n";
                return 1;
            }
            backendPort = atoi(argv[++i]);
            if (backendPort <= 0 || backendPort > 65535) {
                cerr << "Error: invalid backend port\n";
                return 1;
            }
        }
        else {
            cerr << "Error: unknown option: " << arg << "\n";
            usage(argv[0]);
            return 1;
        }
    }

    if (useConfig) {
        cout << "Using config file: " << configPath << endl;
        // TODO: parse JSON config here
        // then override backendHost / backendPort from config
        // unless CLI args already replaced them
    }

    cout << "Backend host: " << backendHost << endl;
    cout << "Backend port: " << backendPort << endl;

    ProxyServer proxy(backendPort, const_cast<char*>(backendHost.c_str()));
    proxy.mainloop();

    return 0;
}