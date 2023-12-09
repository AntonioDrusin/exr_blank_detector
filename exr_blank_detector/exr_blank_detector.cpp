#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include "arguments.h"

namespace fs = std::filesystem;

void display_files_recursively(const std::string& folderPath) {
  
    if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
        std::cerr << "Path is not a valid directory: " << folderPath << std::endl;
        return;
    }

    try {
        for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
            if (fs::is_regular_file(entry.status())) {
                std::cout << entry.path() << std::endl;
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Standard exception: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
    }
}


int main(const int argc, char* argv[])
{
    const auto args = new arguments(argc, argv);
    const auto folders = args->get_folders();

    if (folders.empty())
    {
        std::cerr << "No folder specified." << std::endl;
        std::cerr << "Specify folders with the -f command line arguments. For example:" << std::endl;
        std::cerr << R"(exr_blank_detector -f "C:\My Folder")" << std::endl;
        return -1;
    }
    
    
    for (const auto& folder : folders)
    {
        std::cerr << "Processing folder: " << folder << std::endl;
        display_files_recursively(folder);
    }
    
}
