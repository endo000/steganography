//
// Created by nsakn on 12/15/2022.
//

#ifndef STEGANOGRAFIA_STEGANALYSIS_H
#define STEGANOGRAFIA_STEGANALYSIS_H

#include <vector>
#include <cstdint>
#include <opencv2/core/mat.hpp>
#include "../Configuration.h"

std::vector<uint8_t> steganalysis(cv::Mat &hided_image, Configuration &config);

void read_header(cv::Mat &image, Configuration &config);

int64_t read_seed(cv::Mat &image, uint32_t &pixel_pos, const Configuration &config, uint8_t &byte, uint8_t &bit_index);

uint32_t read_length(cv::Mat &image, uint32_t &pixel_pos, const Configuration &config, uint8_t &byte, uint8_t &bit_index,
                     std::vector<uint32_t> &random_positions);

std::vector<uint8_t>
read_message(cv::Mat &image, uint32_t &pixel_pos, const Configuration &config, uint8_t &byte, uint8_t &bit_index,
             const uint32_t &length, std::vector<uint32_t> &random_positions);

#endif //STEGANOGRAFIA_STEGANALYSIS_H
