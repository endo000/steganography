#include <opencv2/opencv.hpp>
#include <vector>
#include "Configuration.h"
#include "utils/utils.h"
#include "steganography/steganography.h"
#include "steganalysis/steganalysis.h"
#include "openmpi/OpenMpiConfiguration.h"
#include "openmpi/utils/utils.h"
#include "openmpi/RankData.h"
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi/timer.hpp>
#include <boost/mpi.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/bitset.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <chrono>

using namespace std::chrono;

namespace boost {
    namespace serialization {

        template<class Archive>
        void serialize(Archive &ar, RankData &rd, const unsigned int version) {
            ar & rd.image_name;
            ar & rd.text;
        }

        template<class Archive>
        void serialize(Archive &ar, OpenMpiConfiguration &config, const unsigned int version) {
            ar & config.hide;
            ar & config.images;
            ar & config.images_dir;
            ar & config.book;
            ar & config.output_dir;
            ar & config.properties;
        }

    } // namespace serialization
} // namespace boost



int main(int argc, char **argv) {
    boost::mpi::environment env;
    boost::mpi::communicator world;

    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);

    printf("Host: %s\n", hostname);
    world.barrier();

    // printf("Wait 5 sec before start\n");
    // world.barrier();
    // boost::mpi::timer start_time = boost::mpi::timer();
    // while (start_time.elapsed() <= 5) {
    // }
    // printf("Start program\n");
    // world.barrier();

    OpenMpiConfiguration config;
    std::vector <std::vector<RankData>> ranks_data;
    std::vector <RankData> rank_data;

    auto start = high_resolution_clock::now();
    if (world.rank() == 0) {
        config = openmpi_read_flags(argc, argv);

        std::vector <std::string> image_files = read_images(config);

        if (config.hide) {
            std::vector <uint8_t> book = read_book(config);

            std::vector <cv::Mat> images = files_to_mats(image_files);
            if (!openmpi_check_size(images, book, config)) {
                std::cout << "Can't fit book into the images" << std::endl;
                exit(1);
            }

            ranks_data = assign_images_to_ranks(world, image_files);
            assign_book_to_ranks(world, ranks_data, book, image_files);

            boost::filesystem::create_directories(config.output_dir);
        } else {
            std::sort(image_files.begin(), image_files.end(), compare_image_files);

            ranks_data = assign_images_to_ranks(world, image_files);
        }
    }
    world.barrier();


    boost::mpi::broadcast(world, config, 0);
    boost::mpi::scatter(world, ranks_data, rank_data, 0);

    Configuration rank_config;
    rank_config.properties = config.properties;
    rank_config.VERBOSE = false;

    if (config.hide) {
        int i = 0;
        for (auto &rd: rank_data) {
            rank_config.image = rd.image_name;
            cv::Mat image = read_image(rank_config);

            cv::Mat hidden_image = steganography(image, rd.text, rank_config);

            boost::filesystem::path output_path(config.output_dir);
            boost::filesystem::path image_path(rd.image_name);

            boost::format output_name("%1%_%2%_%3%");
            output_name % world.rank();
            output_name % i++;
            output_name % image_path.filename().string();

            output_path /= output_name.str();

            cv::imwrite(output_path.string(), hidden_image);
        }

    } else {
        for (auto &rd: rank_data) {
            rank_config.image = rd.image_name;
            cv::Mat image = read_image(rank_config);

            rd.text = steganalysis(image, rank_config);
        }

        boost::mpi::gather(world, rank_data, ranks_data, 0);

        std::vector <uint8_t> book;
        if (world.rank() == 0) {
            for (const auto &rsd: ranks_data) {
                for (const auto &rd: rsd) {
                    book.insert(book.end(), rd.text.begin(), rd.text.end());
                }
            }

            write_book(book, config);
        }
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    if (world.rank() == 0) {
        std::cout << duration.count() << std::endl;
    }

    return 0;
}
