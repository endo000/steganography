//
// Created by nsakn on 12/21/2022.
//

#ifndef STEGANOGRAFIA_OPENMPIUTILS_H
#define STEGANOGRAFIA_OPENMPIUTILS_H

#include "../OpenMpiConfiguration.h"
#include "../RankData.h"
#include <boost/mpi/communicator.hpp>

OpenMpiConfiguration openmpi_read_flags(int argc, char **argv);

std::vector<std::string> read_images(const OpenMpiConfiguration &config);

std::vector<uint8_t> read_book(const OpenMpiConfiguration &config);

void write_book(const std::vector<uint8_t> &book, const OpenMpiConfiguration &config);

std::vector<cv::Mat> files_to_mats(const std::vector<std::string> &image_files);

bool openmpi_is_random(const OpenMpiConfiguration &config);

bool openmpi_is_all_channels(const OpenMpiConfiguration &config);

uint8_t openmpi_get_bits(const OpenMpiConfiguration &config);

bool openmpi_check_size(const std::vector<cv::Mat> &images, const std::vector<uint8_t> &book,
                        const OpenMpiConfiguration &config);

std::vector<std::vector<RankData>>
assign_images_to_ranks(const boost::mpi::communicator &world, const std::vector<std::string> &image_files);

void assign_book_to_ranks(const boost::mpi::communicator &world, std::vector<std::vector<RankData>> &ranks_data,
                          const std::vector<uint8_t> &book, const std::vector<std::string> &image_files);

bool compare_image_files(const std::string &first, const std::string &second);

#endif //STEGANOGRAFIA_OPENMPIUTILS_H
