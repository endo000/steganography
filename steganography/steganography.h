//
// Created by nsakn on 12/15/2022.
//

#ifndef STEGANOGRAFIA_STEGANOGRAPHY_H
#define STEGANOGRAFIA_STEGANOGRAPHY_H

#include <opencv2/core/mat.hpp>
#include "../Configuration.h"

cv::Mat
steganography(const cv::Mat& original_image, std::vector<uint8_t> &message, const Configuration &config);

void write_header(cv::Mat &image, const std::vector<uint8_t> &message, uint32_t &offset, const Configuration &config);

void write_seed(cv::Mat &image, const std::vector<uint8_t> &message, uint32_t &offset, const Configuration &config,
                uint32_t &pixel_pos);

void write_in_pixel(cv::Vec3b &pixel, const std::vector<uint8_t> &message, uint32_t &offset, uint32_t end,
                    const int8_t channel_index, const uint8_t &bits, const Configuration &config);

#endif //STEGANOGRAFIA_STEGANOGRAPHY_H
