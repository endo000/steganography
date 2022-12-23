//
// Created by nsakn on 12/15/2022.
//

#ifndef STEGANOGRAFIA_OPENMPI_UTILS_H
#define STEGANOGRAFIA_OPENMPI_UTILS_H

#include <vector>
#include <cstdint>
#include "../Configuration.h"

cv::Mat read_image(const Configuration &config);

void write_image(const Configuration &config, const cv::Mat &image);

std::vector<uint8_t> read_message_file(const Configuration &config);

void message_write_file(const std::vector<uint8_t> &message, const Configuration &config);

uint32_t calculate_length(const std::vector<uint8_t> &message, const Configuration &config);

bool check_sizes(const cv::Mat &image, const uint32_t &length, const Configuration &config);

cv::Vec3b &get_pixel_random(cv::Mat &image, std::vector<uint32_t> &random_positions, const Configuration &config);

cv::Vec3b &
get_pixel(cv::Mat &image, uint32_t &pixel_pos);

void set_pixel(cv::Mat &image, uint32_t &pixel_pos, const cv::Vec3b &pixel);

bool get_message_bit(const std::vector<uint8_t> &message, uint32_t &offset);

void
read_pixel(const cv::Vec3b &pixel, const int8_t &channel_index, const uint8_t &bits, std::vector<uint8_t> &message,
           uint8_t &byte, uint8_t &bit_index, const Configuration &config);

void message_add_header(std::vector<uint8_t> &message, const Configuration &config, const uint64_t &seed);

#endif //STEGANOGRAFIA_OPENMPI_UTILS_H
