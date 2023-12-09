#pragma once
#include <string>
#include <vector>

class arguments
{
    std::vector<std::string> folder_;
    std::string wildcard_;
public:
    arguments(const int argc, char** argv);
    std::vector<std::string> get_folders();
    std::string get_wildcard();
};

