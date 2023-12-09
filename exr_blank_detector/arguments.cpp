#include <string>
#include <vector>
#include "arguments.h"
#include <iostream>

arguments::arguments(const int argc, char** argv)
{
    for (int i = 0; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg == "-f")
        {
            if (i < argc - 1)
            {
                folder_.emplace_back(argv[++i]);
            }
            else
            {
                std::cerr << "No folder specified after '-f' command line argument." << std::endl;
            }
        }

        if (arg == "-w")
        {
            if (i < argc - 1)
            {
                wildcard_ = argv[++i];
            }
            else
            {
                std::cerr << "No wildcard specified after '-w' command line argument." << std::endl;
            }
        }
    }
}

std::vector<std::string> arguments::get_folders()
{
    return folder_;
}

std::string arguments::get_wildcard()
{
    return wildcard_;
}
