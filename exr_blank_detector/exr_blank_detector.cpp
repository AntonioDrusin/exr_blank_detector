#define IMATH_DLL

#include <iostream>
#include <filesystem>
#include <fstream>
#include <future>
#include <regex>
#include <string>
#include "arguments.h"
#include "ImfRgbaFile.h"
#include "ImfArray.h"

namespace fs = std::filesystem;
std::mutex cout_mutex;
void safePrint(const std::string& message) {
    std::lock_guard<std::mutex> guard(cout_mutex);
    std::cout << message << std::endl;
}

void make_file(const std::string& folderPath)
{
    try
    {
        constexpr int width = 10;
        constexpr int height = 10;

        Imf::Array2D<Imf::Rgba> pixels(width, height);
        for (int y = 0; y < height; y++)
            for (int x = 0; x < width; x++)
                pixels[y][x] = Imf::Rgba(0, x / (width - 1.0f), y / (height - 1.0f));

        const auto file_name = folderPath + "\\hello.exr";
        Imf::RgbaOutputFile file(file_name.c_str(), width, height, Imf::WRITE_RGBA);
        file.setFrameBuffer(&pixels[0][0], 1, width);
        file.writePixels(height);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Unable to read image file hello.exr:" << e.what() << std::endl;
    }
}

bool are_all_pixels_black(const std::string& filename)
{
    try
    {
        Imf::RgbaInputFile file(filename.c_str());
        const Imath::Box2i dw = file.dataWindow();

        const int width = dw.max.x - dw.min.x + 1;
        const int height = dw.max.y - dw.min.y + 1;

        std::vector<Imf::Rgba> pixels(height * width);
        file.setFrameBuffer(pixels.data() - dw.min.x - dw.min.y * width, 1, width);
        file.readPixels(dw.min.y, dw.max.y);

        float zero = 0.0;

        for (const auto& pixel : pixels)
        {
            if (pixel.r != zero || pixel.g != zero || pixel.b != zero)
            {
                return false; // Found a non-black pixel
            }
        }

        return true; // All pixels are black
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return false;
    }
}

void wait_for_available_slot(const std::vector<std::future<void>>& futures, size_t max_concurrent_tasks)
{
    while (futures.size() >= max_concurrent_tasks)
    {
        for (auto& future : futures)
        {
            if (future.wait_for(std::chrono::milliseconds(100)) == std::future_status::ready)
            {
                return; // One future completed, slot available
            }
        }
    }
}


std::string dos_wildcard_to_regex(const std::string& wildcard) {
    std::string regex_str;
    regex_str.reserve(wildcard.size() * 2);  // Reserve enough space to avoid frequent reallocations
    
    
    for (const char ch : wildcard) {
        switch (ch) {
        case '*': regex_str += ".*"; break;
        case '?': regex_str += '.'; break;
            // Escape special regex characters
            case '.': case '+': case '\\': case '{': case '}':
            case '(': case ')': case '[': case ']': case '^': case '$':
            case '|':
                regex_str += '\\';
            regex_str += ch;
            break;
        default:
            regex_str += ch;
        }
    }

    return regex_str;
}

bool string_matches_regex(const std::string& str, const std::string& pattern) {
    const std::regex re(pattern, std::regex_constants::icase);
    return std::regex_match(str, re);
}

void analyze_files_recursively(const std::string& folder_path, const std::string &wildcard)
{
    if (!fs::exists(folder_path) || !fs::is_directory(folder_path))
    {
        std::cerr << "Path is not a valid directory: " << folder_path << std::endl;
        return;
    }

    std::vector<std::future<void>> futures;
    const size_t max_concurrent_tasks = std::thread::hardware_concurrency();

    try
    {
        for (const auto& entry : fs::recursive_directory_iterator(folder_path))
        {
            //safePrint(entry.path().string());
           
            if (fs::is_regular_file(entry.status()))
            {
                wait_for_available_slot(futures, max_concurrent_tasks);
                const auto file_name = entry.path().filename().string();

                if (!wildcard.empty() &&
                    !string_matches_regex(file_name, dos_wildcard_to_regex(wildcard))
                    )
                {
                    continue;
                }
                
                auto future = std::async(std::launch::async, [entry]()
                {
                    const auto black = are_all_pixels_black(entry.path().string());
                    safePrint("result|" + entry.path().string() + "|" + (black ? "black" : "not_black"));
                });
                
                futures.push_back(std::move(future));

                // Remove completed futures
                futures.erase(
                    std::ranges::remove_if(futures, 
                                           [](const std::future<void>& f) { return f.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready; }).begin(),
                    futures.end());
            }
        }

        // Wait for all remaining tasks to complete
        for (auto& future : futures) {
            future.wait();
        }
    }
    catch (const fs::filesystem_error& e)
    {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    } catch (const std::exception& e)
    {
        std::cerr << "Standard exception: " << e.what() << std::endl;
    } catch (...)
    {
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
        analyze_files_recursively(folder, args->get_wildcard());
    }

    return 0;
}
