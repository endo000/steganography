#include <vector>
#include <cstdint>
#include <fstream>
#include <iostream>
//#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "../Configuration.h"
#include "config_utils.h"

cv::Mat read_image(const Configuration &config) {
    std::ifstream file;
    file.open(config.image);

    if (!file) {
        std::cout << "Image doesnt exist" << std::endl;
        exit(1);
    }
    cv::Mat img = cv::imread(config.image, cv::IMREAD_COLOR);

    return img;
}

void write_image(const Configuration &config, const cv::Mat &image) {
    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    compression_params.push_back(100);

    cv::imwrite(config.output, image);
}

std::vector<uint8_t> read_message_file(const Configuration &config) {
    std::ifstream file(config.message, std::ios::binary);
    if (!file) {
        std::cout << "Message doesn't exist" << std::endl;
        exit(1);
    }
    std::vector<uint8_t> vec(std::istreambuf_iterator<char>(file), {});
    return vec;
}

void message_write_file(const std::vector<uint8_t> &message, const Configuration &config) {
    std::ofstream file(config.output, std::ios::binary);
    file.write((char *) &message[0], (long long) (message.size() * sizeof(uint8_t)));
    file.close();
}

// 3 bits - r(1) g(2) b(4), all(0)
// 3 bits - bits per channel
// 1 bit - random
// pad 1 bit
// 8 bits

// 4 * 8 (32) - message length
// n * 8 - one byte(character) of message
uint32_t calculate_length(const std::vector<uint8_t> &message, const Configuration &config) {
    uint32_t length = 8 + 32 + message.size() * 8;

    // 64 bits - seed if random
    if (is_random(config)) {
        length += 8 * 8;
    }

    return length;
}

bool check_sizes(const cv::Mat &image, const uint32_t &length, const Configuration &config) {
    uint64_t image_size = image.rows * image.cols; // pixels

    if (config.VERBOSE) {
        printf("Image size: %ld, length: %d\n", image_size, length);
    }

    if (image_size > length) {
        return true;
    }

    if (is_all_channels(config)) {
        image_size *= 3;

        if (config.VERBOSE) {
            printf("Image size(all channels): %ld, length: %d\n", image_size, length);
        }

        if (image_size > length) {
            return true;
        }
    }


    uint8_t bits_for_pixel = get_bits(config);
    image_size *= bits_for_pixel;

    if (config.VERBOSE) {
        printf("Image size(%d bits): %ld, length: %d\n", bits_for_pixel, image_size, length);
    }

    if (image_size > length) {
        return true;
    }

    return false;
}

cv::Vec3b &get_pixel_random(cv::Mat &image, std::vector<uint32_t> &random_positions, const Configuration &config) {
    uint32_t pixel_pos;
    std::vector<uint32_t>::iterator it;
    do {
        pixel_pos = rand() % (image.rows * image.cols);

        if (config.VERBOSE)
            printf("Trying pixel %d\n", pixel_pos);

        it = std::find(random_positions.begin(), random_positions.end(), pixel_pos);
    } while (it != random_positions.end());
    random_positions.push_back(pixel_pos);

    int32_t i = (int32_t) pixel_pos / image.rows;
    int32_t j = (int32_t) pixel_pos % image.rows;

    return image.at<cv::Vec3b>(i, j);
}

cv::Vec3b &
get_pixel(cv::Mat &image, uint32_t &pixel_pos) {
    int32_t i = (int32_t) pixel_pos / image.rows;
    int32_t j = (int32_t) pixel_pos % image.rows;
    pixel_pos++;

    return image.at<cv::Vec3b>(i, j);
}


void
set_pixel(cv::Mat &image, uint32_t &pixel_pos, const cv::Vec3b &pixel) {
    int32_t i = (int32_t) pixel_pos / image.rows;
    int32_t j = (int32_t) pixel_pos % image.rows;

    image.at<cv::Vec3b>(i, j) = pixel;
}

bool get_message_bit(const std::vector<uint8_t> &message, uint32_t &offset) {
    uint32_t row = offset / 8;
    uint32_t bit = offset % 8;
    offset++;
    return (message[row] >> bit) & 1;
}

void
read_pixel(const cv::Vec3b &pixel, const int8_t &channel_index, const uint8_t &bits, std::vector<uint8_t> &message,
           uint8_t &byte, uint8_t &bit_index, const Configuration &config) {
    if (channel_index == -1) {
        for (int ch = 0; ch < 3; ++ch) {
            for (int bit_i = 0; bit_i < bits; ++bit_i) {
                bool bit = (pixel[ch] >> bit_i) & 1;
                if (config.VERBOSE)
                    printf("%d ", bit);

                byte |= bit << bit_index;
                bit_index++;
                if (bit_index == 8) {
                    bit_index = 0;
                    message.push_back(byte);
                    byte = 0;
                }
            }
        }
    } else {
        for (int bit_i = 0; bit_i < bits; ++bit_i) {
            bool bit = (pixel[channel_index] >> bit_i) & 1;
            byte |= bit << bit_index;
            if (config.VERBOSE)
                printf("%d ", bit);

            bit_index++;
            if (bit_index == 8) {
                bit_index = 0;
                message.push_back(byte);
                byte = 0;
            }
        }
    }
    if (config.VERBOSE)
        printf("\n");
}

void message_add_header(std::vector<uint8_t> &message, const Configuration &config, const uint64_t &seed) {
    uint8_t buffer[4] = {0};
    uint32_t size = message.size();
    memcpy((uint8_t *) buffer, (uint8_t *) &size, sizeof(int));
    for (unsigned char &i: buffer) {
        message.insert(message.begin(), i);
    }

    if (is_random(config)) {
        uint8_t buffer_64[8] = {0};
        memcpy((uint8_t *) buffer_64, (uint8_t *) &seed, sizeof(uint64_t));
        for (unsigned char &i: buffer_64) {
            message.insert(message.begin(), i);
        }
    }

    // Use (i - 1) because 1st bit is for hide/reveal
    uint8_t header = 0;
    for (int i = 1; i < 8; ++i) {
        header |= config.properties[i] << (i - 1);
    }

    message.insert(message.begin(), header);
}
