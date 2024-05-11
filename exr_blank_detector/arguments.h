#pragma once
#include <string>
#include <vector>

class arguments
{
    std::vector<std::string> folder_;
    std::vector<std::string> skip_folder_;
    std::vector<std::string> skip_file_;
    std::string wildcard_;
public:
    arguments(const int argc, char** argv);
    std::vector<std::string> get_folders();
    std::vector<std::string> get_skip_files();
    std::vector<std::string> get_skip_folders();
    std::string get_wildcard();
};



