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

        if (arg == "-r")
        {
            if (i < argc - 1)
            {
                skip_folder_.emplace_back(argv[++i]);
            }
            else
            {
                std::cerr << "No folder wildcard exclusion specified after '-r' command line argument." << std::endl;
            }
        }

        if (arg == "-x")
        {
            if (i < argc - 1)
            {
                skip_file_.emplace_back(argv[++i]);
            }
            else
            {
                std::cerr << "No file exclusion wildcard specified after '-x' command line argument." << std::endl;
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

std::vector<std::string> arguments::get_skip_folders()
{
    return skip_folder_;
}


std::vector<std::string> arguments::get_skip_files()
{
    return skip_file_;
}


std::string arguments::get_wildcard()
{
    return wildcard_;
}
