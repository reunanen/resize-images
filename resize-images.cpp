#include <dlib/dir_nav/dir_nav_extensions.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include "cxxopts/include/cxxopts.hpp"

int main(int argc, char** argv)
{
    if (argc == 1) {
        std::cout << "Usage: " << std::endl;
        std::cout << "> resize-images -d=/path/to/images -s=0.5" << std::endl;
        return 1;
    }

    cxxopts::Options options("resize-images", "Batch-resize images");

    options.add_options()
        ("d,directory", "The directory where to search for input files", cxxopts::value<std::string>())
        ("s,scaling-factor", "Try >1 to upscale, and <1 to downscale", cxxopts::value<double>())
        ("i,interpolation-method", "Try 0 for nearest neighbor, 1 for bilinear, 2 for bicubic, 3 for pixel area, or 4 for Lanczos", cxxopts::value<int>()->default_value("1"))
        ;

    try {
        options.parse(argc, argv);

        cxxopts::check_required(options, { "directory", "scaling-factor" });

        const std::string directory = options["directory"].as<std::string>();
        const double scaling_factor = options["scaling-factor"].as<double>();
        const int interpolation_method = options["interpolation-method"].as<int>();

        std::cout << "Searching for images in " << directory << " ..." << std::endl;

        const std::vector<dlib::file> files = dlib::get_files_in_directory_tree(directory, dlib::match_endings(".jpeg .jpg .png"));

        std::cout << "Found " << files.size() << " files, now resizing ..." << std::endl;

        for (const auto& file : files) {

            const std::string& full_name = file.full_name();

            std::cout << "Processing " << full_name;

            cv::Mat image = cv::imread(full_name, cv::IMREAD_UNCHANGED);

            if (!image.data) {
                std::cout << " - unable to read, skipping...";
            }
            else {
                std::cout
                    << ", width = " << image.cols
                    << ", height = " << image.rows
                    << ", channels = " << image.channels()
                    << ", type = 0x" << std::hex << image.type();

                cv::resize(image, image, cv::Size(), scaling_factor, scaling_factor, interpolation_method);

                cv::imwrite(full_name, image);
            }

            std::cout << std::endl;
        }

        return 0;
    }
    catch (std::exception& e) {
        std::cerr << std::endl << "Error: " << e.what() << std::endl;
        std::cerr << std::endl << options.help() << std::endl;
        return 255;
    }
}
