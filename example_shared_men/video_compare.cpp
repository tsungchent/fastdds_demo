#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <cxxopts.hpp>

#include <filesystem>

int main(int argc, char *argv[])
{
    cxxopts::Options options("video compare program", "tool to compare videos");
    options.add_options()("p, path", "video path",
                          cxxopts::value<std::string>()->default_value("E:\\video_download\\VID_20200609_163208_00_118.mp4"),
                          "first video path")("r, refpath",
                                        "ref video path",
                                        cxxopts::value<std::string>()->default_value("E:\\video_download\\VID_20200609_163208_00_118_plus.mp4"),
                                        "second video path")("m,mode",
                                                      "compare mode, support: or, and, xor, vcat, hvat",
                                                      cxxopts::value<std::string>()->default_value("xor"),
                                                      "compare mode, support: or, and, xor, vcat, hvat")("h,help", "Print usage");

    std::cout << "this is image transform test." << std::endl;

    try
    {
        std::cout << "begin to parse: " << std::endl;
        auto result = options.parse(argc, argv);
        if (result.count("help"))
        {
            std::cout << options.help() << std::endl;
            exit(0);
        }
 
        std::string compare_mode = result["mode"].as<std::string>();
        std::string video_path = result["path"].as<std::string>();
        std::string ref_video_path = result["refpath"].as<std::string>();

        std::function<cv::Mat(const cv::Mat&, const cv::Mat&)> op_func;
        
        if(compare_mode == "xor") {
            op_func = [](const cv::Mat &mat1, const cv::Mat &mat2) {
                return mat1 ^ mat2;
            };
        } else if(compare_mode == "or") {
            op_func = [](const cv::Mat &mat1, const cv::Mat &mat2) {
                return mat1 | mat2;
            };
        } else if(compare_mode == "and") {
            op_func = [](const cv::Mat &mat1, const cv::Mat &mat2) {
                return mat1 & mat2;
            };
        } else if(compare_mode == "vcat") {
            op_func = [](const cv::Mat &mat1, const cv::Mat &mat2) {
                cv::Mat out;
                cv::vconcat(std::vector<cv::Mat>{mat1, mat2}, out) ;
                return out;
            };
        } else if(compare_mode == "hcat") {
            op_func = [](const cv::Mat &mat1, const cv::Mat &mat2) {
                cv::Mat out;
                cv::vconcat(std::vector<cv::Mat>{mat1, mat2}, out) ;
                return out;
            };
        } else {
           std::cout << compare_mode << " is not supported." << std::endl;
           std::cout << options.help() << std::endl;
           exit(-1);
        }
        
        std::cout << "read first video frame: " << std::endl;
        cv::VideoCapture cap1(video_path);
        cv::VideoCapture cap2(ref_video_path);

        std::filesystem::path ref_path(ref_video_path);
        std::string merge_path = std::regex_replace(video_path, std::regex("\\.[\\w]+"), "");
        std::cout << "result path: " << merge_path << std::endl;

        merge_path = merge_path + "_" + compare_mode + "_" + ref_path.filename().generic_string();
        // std::filesystem::path merge_path;
        std::cout << "result path: " << merge_path << std::endl;

        cv::Mat image1, image2;
        cap1 >> image1;
        cap2 >> image2;

        if(image1.size() != image2.size() || image1.channels() != image2.channels()) {
            std::cout << "image size or channel is not matched." << std::endl;
        }

        if (!image1.empty() && !image2.empty()) {
            cv::Mat image = op_func(image1, image2);
            cv::VideoWriter writer(merge_path, cap1.get(cv::CAP_PROP_FOURCC), cap1.get(cv::CAP_PROP_FPS), cv::Size(image.cols, image.rows));
            writer << image;
            cap1 >> image1;
            cap2 >> image2;

            while(!image1.empty() && !image2.empty()) {
                image = op_func(image1, image2);
                writer << image;
                cap1 >> image1;
                cap2 >> image2; 
            }
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