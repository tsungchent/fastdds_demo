#include <stdio.h>
#include <cxxopts.hpp>
#include <transport_test.h>
#include <regex>
#include <filesystem>

int main(int argc, char *argv[])
{
    cxxopts::Options options("transport_test", "this is a simple fastdds shared memory to test transport");
    options.add_options()("t, type", "host or slave",
                          cxxopts::value<std::string>()->default_value("slave"),
                          "host/slave")("p, path",
                                        "image path",
                                        cxxopts::value<std::string>()->default_value("E:\\video_download\\VID_20200609_163208_00_118.mp4"),
                                        "image path")("f, fps",
                                                      "send fps for host and save fps for slave",
                                                      cxxopts::value<int>()->default_value("30"),
                                                      "send fps for host and save fps for slave")("h,help", "Print usage");
    std::cout << "this is image transform test." << std::endl;

    try
    {
        auto result = options.parse(argc, argv);
        if (result.count("help"))
        {
            std::cout << options.help() << std::endl;
            exit(0);
        }

        std::string mode = result["type"].as<std::string>();
        std::string image_path = result["path"].as<std::string>();
        int fps = result["fps"].as<int>();

        if (mode == "host")
        {
            send_image_test(image_path, fps);
        }
        else
        {
            if (std::filesystem::exists(image_path))
            {
                std::regex reg("\\.");
                // std::string origin_path = image_path;
                image_path = std::regex_replace(image_path, reg, "_plus.");
            }
            std::cout << "save pathname: " << image_path << std::endl;
            receive_image_test(image_path, fps);
        }
    }
    catch (...)
    {
        std::cout << "invalid input...." << std::endl;
        std::cout << options.help() << std::endl;
        exit(-1);
    }

    return 0;
}