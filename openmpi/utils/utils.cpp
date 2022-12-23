//
// Created by nsakn on 12/21/2022.
//

#include <cstring>
#include <iostream>
#include <fstream>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/directory.hpp>
#include "utils.h"

OpenMpiConfiguration openmpi_read_flags(int argc, char **argv) {
    OpenMpiConfiguration config;

    uint8_t required_flags = 0;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-v") == 0) {
            config.VERBOSE = true;
        }
        if (strcmp(argv[i], "-h") == 0) {
            config.hide = true;

            required_flags |= 1;
        } else if (strcmp(argv[i], "-u") == 0) {
            config.hide = false;

            required_flags |= 1;
            required_flags |= 1 << 3;
        } else if (strcmp(argv[i], "-i") == 0) {
            i++;
            config.images_dir = argv[i];

            required_flags |= 1 << 1;
        } else if (strcmp(argv[i], "-b") == 0) {
            i++;
            config.book = argv[i];

            required_flags |= 1 << 2;
        } else if (strcmp(argv[i], "-o") == 0) {
            i++;
            config.output_dir = argv[i];

            required_flags |= 1 << 3;
        } else if (strcmp(argv[i], "-r") == 0) {
            config.properties[4] = true;
            config.properties[5] = false;
            config.properties[6] = false;
        } else if (strcmp(argv[i], "-g") == 0) {
            config.properties[4] = false;
            config.properties[5] = true;
            config.properties[6] = false;
        } else if (strcmp(argv[i], "-b") == 0) {
            config.properties[4] = false;
            config.properties[5] = false;
            config.properties[6] = true;
        } else if (strcmp(argv[i], "--bits") == 0) {
            i++;
            int bits = std::stoi(argv[i]);
            if (0 > bits || bits > 4) {
                std::cout << "Wrong bits number" << std::endl;
                exit(1);
            }

            for (int j = 0; j < 3; ++j) {
                config.properties[1 + j] = (bits >> j) & 1;
            }
        } else if (strcmp(argv[i], "--random") == 0) {
            config.properties[7] = true;
        }

    }

    if (required_flags != 0b1111) {
        std::cout << "Not all flags passed" << std::endl;
        exit(1);
    }

    return config;
}

std::vector<std::string> read_images(const OpenMpiConfiguration &config) {
    std::vector<std::string> images;

    if (config.VERBOSE) {
        printf("Images directory: %s\n", config.images_dir.c_str());
    }

    boost::filesystem::path dir(config.images_dir);

    boost::filesystem::directory_iterator end_it;
    for (boost::filesystem::directory_iterator it(dir); it != end_it; ++it) {
        std::string current_file = it->path().string();
        if (config.VERBOSE) {
            printf("Found: %s\n", current_file.c_str());
        }
        images.push_back(current_file);
    }

    return images;
}

std::vector<uint8_t> read_book(const OpenMpiConfiguration &config) {
    std::ifstream file(config.book, std::ios::binary);
    if (!file) {
        std::cout << "Book doesn't exist" << std::endl;
        exit(1);
    }
    std::vector<uint8_t> book(std::istreambuf_iterator<char>(file), {});

    if (config.VERBOSE) {
        printf("Read book, size %ld\n", book.size());
    }
    return book;
}

void write_book(const std::vector<uint8_t> &book, const OpenMpiConfiguration &config) {
    std::ofstream file(config.book, std::ios::binary);
    file.write((char *) &book[0], (long long) (book.size() * sizeof(uint8_t)));
    file.close();
}

std::vector<cv::Mat> files_to_mats(const std::vector<std::string> &image_files) {
    std::vector<cv::Mat> mats;
    for (const auto &filename: image_files) {
        mats.push_back(cv::imread(filename, cv::IMREAD_COLOR));

        if (mats.back().empty()) {
            std::cout << "Image doesn't exist" << std::endl;
            exit(1);
        }
    }

    return mats;
}

bool openmpi_is_random(const OpenMpiConfiguration &config) {
    return config.properties[7];
}

bool openmpi_is_all_channels(const OpenMpiConfiguration &config) {
    if (config.properties[4] || config.properties[5] || config.properties[6]) {
        return false;
    }

    return true;
}

uint8_t openmpi_get_bits(const OpenMpiConfiguration &config) {
    uint8_t bits = 0;
    bits |= config.properties[1];
    bits |= config.properties[2] << 1;
    bits |= config.properties[3] << 2;

    if (bits == 0) {
        return 1;
    }

    return bits;
}

bool openmpi_check_size(const std::vector<cv::Mat> &images, const std::vector<uint8_t> &book,
                        const OpenMpiConfiguration &config) {
    uint32_t length = book.size() * 8;

    uint64_t images_size = 0;
    for (const auto &image: images) {
        images_size += image.rows * image.cols;

        length += 8 + 32;
        if (openmpi_is_random(config)) {
            length += 8 * 8;
        }
    }

    if (images_size > length) {
        return true;
    }

    if (openmpi_is_all_channels(config)) {
        images_size *= 3;

        if (images_size > length) {
            return true;
        }
    }

    uint8_t bits_for_pixel = openmpi_get_bits(config);
    images_size *= bits_for_pixel;

    if (images_size > length) {
        return true;
    }

    return false;
}

std::vector<std::vector<RankData>>
assign_images_to_ranks(const boost::mpi::communicator &world, const std::vector<std::string> &image_files) {
    std::vector<std::vector<RankData>> ranks_data = std::vector<std::vector<RankData>>(world.size(),
                                                                                       std::vector<RankData>());
    int used_ranks;
    if (world.size() > (int) image_files.size()) {
        used_ranks = (int) image_files.size();
    } else {
        used_ranks = world.size();
    }

    int images_per_rank = (int) image_files.size() / world.size();
    int left_images = (int) image_files.size() % world.size();

    int image_index = 0;
    for (int i = 0; i < used_ranks; ++i) {
        for (int j = 0; j < images_per_rank; ++j) {
            RankData data;
            data.image_name = image_files[image_index];

            ranks_data[i].push_back(data);
            image_index++;
        }

        if (left_images > 0) {
            RankData data;
            data.image_name = image_files[image_index];

            ranks_data[i].push_back(data);
            image_index++;
            left_images--;
        }
    }

    return ranks_data;
}

void assign_book_to_ranks(const boost::mpi::communicator &world, std::vector<std::vector<RankData>> &ranks_data,
                          const std::vector<uint8_t> &book, const std::vector<std::string> &image_files) {

    int used_ranks;
    if (world.size() > (int) image_files.size()) {
        used_ranks = (int) image_files.size();
    } else {
        used_ranks = world.size();
    }
    int images_per_rank = (int) image_files.size() / world.size();

    uint64_t book_part_per_image = book.size() / image_files.size();
    uint64_t left_book_part = book.size() % image_files.size();

    auto book_it = book.begin();
    for (int i = 0; i < used_ranks; ++i) {
        for (int j = 0; j < images_per_rank; ++j) {
            RankData &data = ranks_data[i][j];
            if (i == 0 && j == 0) {
                data.text = std::vector<uint8_t>(book_part_per_image + left_book_part);
                std::copy(book_it, book_it + book_part_per_image + left_book_part, data.text.begin());
                book_it += book_part_per_image + left_book_part;
            } else {
                data.text = std::vector<uint8_t>(book_part_per_image);
                std::copy(book_it, book_it + book_part_per_image, data.text.begin());
                book_it += book_part_per_image;
            }

        }
    }
}

bool compare_image_files(const std::string &first, const std::string &second) {
    boost::filesystem::path first_path(first);
    boost::filesystem::path second_path(second);

    std::istringstream first_ss(first_path.filename().string());
    std::istringstream second_ss(second_path.filename().string());

    std::string first_token;
    std::string second_token;
    std::getline(first_ss, first_token, '_');
    std::getline(second_ss, second_token, '_');

    int first_rank = std::stoi(first_token);
    int second_rank = std::stoi(second_token);

    if (first_rank != second_rank) {
        return first_rank < second_rank;
    }

    std::getline(first_ss, first_token, '_');
    std::getline(second_ss, second_token, '_');

    first_rank = std::stoi(first_token);
    second_rank = std::stoi(second_token);

    if (first_rank != second_rank) {
        return first_rank < second_rank;
    }

    std::cout << "Wrong images" << std::endl;
    exit(1);
}