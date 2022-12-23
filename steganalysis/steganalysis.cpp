#include <vector>
#include <cstdint>
#include <opencv2/core/mat.hpp>
#include "../Configuration.h"
#include "../utils/config_utils.h"
#include "steganalysis.h"
#include "../utils/utils.h"

std::vector<uint8_t> steganalysis(cv::Mat &hided_image, Configuration &config) {
    read_header(hided_image, config);

    std::vector<uint8_t> message;
    uint8_t byte = 0;
    uint8_t bit_index = 0;
    uint32_t pixel_pos = 3;
    std::vector<uint32_t> random_positions(0);
    if (is_random(config)) {
        uint64_t seed = read_seed(hided_image, pixel_pos, config, byte, bit_index);
        srand(seed);

        byte = 0;
        bit_index = 0;

        uint32_t length = read_length(hided_image, pixel_pos, config, byte, bit_index, random_positions);
        message = read_message(hided_image, pixel_pos, config, byte, bit_index, length, random_positions);
    } else {
        uint32_t length = read_length(hided_image, pixel_pos, config, byte, bit_index, random_positions);
        message = read_message(hided_image, pixel_pos, config, byte, bit_index, length, random_positions);
    }

    return message;
}

void read_header(cv::Mat &image, Configuration &config) {
    uint8_t bits = 1;
    int8_t channel_index = -1;
    uint32_t end = 8;
    uint8_t properties;
    uint32_t pixel_pos = 0;
    uint32_t offset = 0;

    if (config.VERBOSE)
        printf("Start header\n");

    std::vector<uint8_t> message(0);
    uint8_t byte = 0;
    uint8_t bit_index = 0;
    while (offset < end) {
        cv::Vec3b pixel = get_pixel(image, pixel_pos);
        read_pixel(pixel, channel_index, bits, message, byte, bit_index, config);

        if (config.VERBOSE)
            printf("Pixel %d: [%d, %d, %d]\n", pixel_pos - 1,
                   pixel[0], pixel[1], pixel[2]);

        offset += 3;
    }
    properties = message[0];
    properties &= ~(1 << 7);
    properties <<= 1;
    offset -= 1;

    config.properties = properties;

    if (config.VERBOSE) {
        printf("Properties: %d\n", properties);
        printf("End header\n");
    }
}

int64_t read_seed(cv::Mat &image, uint32_t &pixel_pos, const Configuration &config, uint8_t &byte, uint8_t &bit_index) {
    uint8_t bits = get_bits(config);
    int8_t channel_index = get_channel_index(config);
    int8_t channels = channel_index == -1 ? 3 : 1;
    uint32_t offset = 0;

    std::vector<uint8_t> seed_vec(0);
    while (seed_vec.size() < 8) {
        cv::Vec3b &pixel = get_pixel(image, pixel_pos);
        read_pixel(pixel, channel_index, bits, seed_vec, byte, bit_index, config);

        if (config.VERBOSE) {
            printf("Pixel %d: [%d, %d, %d]\n", pixel_pos - 1,
                   pixel[0], pixel[1], pixel[2]);

            for (uint8_t msg: seed_vec) {
                printf("%d ", msg);
            }
            printf("\n");
        }

        offset += channels * bits;
    }

    int64_t seed = 0;
    for (uint8_t &l: seed_vec) {
        seed <<= 8;
        seed |= l;
    }

    if (config.VERBOSE)
        printf("%ld, End seed\n", seed);

    return seed;
}

uint32_t
read_length(cv::Mat &image, uint32_t &pixel_pos, const Configuration &config, uint8_t &byte, uint8_t &bit_index,
            std::vector<uint32_t> &random_positions) {
    uint8_t bits = get_bits(config);
    int8_t channel_index = get_channel_index(config);
    int8_t channels = channel_index == -1 ? 3 : 1;
    uint32_t offset = 0;

    if (config.VERBOSE) {
        printf("C_I %d\n", channel_index);
        printf("Bits %d\n", bits);
        printf("Start length_vec\n");
    }
    std::vector<uint8_t> length_vec(0);
    while (length_vec.size() < 4) {
        cv::Vec3b pixel;
        if (is_random(config)) {
            pixel = get_pixel_random(image, random_positions, config);
        } else {
            pixel = get_pixel(image, pixel_pos);
        }
        read_pixel(pixel, channel_index, bits, length_vec, byte, bit_index, config);

        int px_pos = is_random(config) ? random_positions.back() : pixel_pos - 1;
        if (config.VERBOSE) {
            printf("Pixel %d: [%d, %d, %d]\n", px_pos,
                   pixel[0], pixel[1], pixel[2]);
            for (uint8_t msg: length_vec) {
                printf("%d ", msg);
            }
            printf("\n");
        }
        offset += channels * bits;
    }

    uint32_t length = 0;
    for (uint8_t &l: length_vec) {
        length <<= 8;
        length |= l;
    }

    if (config.VERBOSE)
        printf("End length_vec\n");
    return length;
}

std::vector<uint8_t>
read_message(cv::Mat &image, uint32_t &pixel_pos, const Configuration &config, uint8_t &byte, uint8_t &bit_index,
             const uint32_t &length, std::vector<uint32_t> &random_positions) {
    uint8_t bits = get_bits(config);
    int8_t channel_index = get_channel_index(config);
    std::vector<uint8_t> message(0);

    if (config.VERBOSE) {
        printf("%d %d %d\n", pixel_pos, byte, bit_index);
        printf("Start message\n");
    }

    while (message.size() < length) {
        cv::Vec3b pixel;
        if (is_random(config)) {
            pixel = get_pixel_random(image, random_positions, config);
        } else {
            pixel = get_pixel(image, pixel_pos);
        }
        read_pixel(pixel, channel_index, bits, message, byte, bit_index, config);

        uint32_t px_pos;
        if (is_random(config)) {
            px_pos = random_positions.back();
        } else {
            px_pos = pixel_pos - 1;
        }
        if (config.VERBOSE) {
            printf("Pixel %d: [%d, %d, %d]\n", px_pos,
                   pixel[0], pixel[1], pixel[2]);

            for (uint8_t msg: message) {
                printf("(%d %c) ", msg, msg);
            }
            printf("\n");
        }
    }

    if (message.size() > length) {
        message.erase(message.begin() + length, message.end());
    }
    if(config.VERBOSE) {
        printf("Total:\n");
        for (uint8_t msg: message) {
            printf("(%d %c) ", msg, msg);
        }
        printf("\n");
        printf("End message\n");
    }

    return message;
}
