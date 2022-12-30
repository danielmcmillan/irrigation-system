#include <iostream>
#include <string>
#include <getopt.h>
#include <cstring>

void remoteCommandRaw(int argc, char **argv);
int vaconSim(int argc, char **argv);

void commands()
{
    std::cerr << "Available commands: remote-command-raw vacon-sim\n";
}

void usage(const char *argv0)
{
    std::cerr << "usage: " << argv0 << " <command> [parameters]\n";
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        commands();
        usage(argv[0]);
    }
    optind = 2;

    if (strcasecmp(argv[1], "remote-command-raw") == 0)
    {
        remoteCommandRaw(argc, argv);
    }
    else if (strcasecmp(argv[1], "vacon-sim") == 0)
    {
        return vaconSim(argc, argv);
    }
    else
    {
        std::cerr << argv[0] << ": invalid command '" << argv[1] << "'\n";
        commands();
        usage(argv[0]);
    }
}
