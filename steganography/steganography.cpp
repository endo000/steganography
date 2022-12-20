#include <opencv2/core/mat.hpp>
#include <iostream>
#include "../Configuration.h"
#include "steganography.h"
#include "../utils/config_utils.h"
#include "../utils/utils.h"

cv::Mat
steganography(const cv::Mat &original_image, std::vector<uint8_t> &message, const Configuration &config) {
    if (message.size() >= UINT32_MAX) {
        std::cout << "Cant hide message in this image (UINT32_MAX)" << std::endl;
        exit(1);
    }
    const std::size_t encoded_length = calculate_length(message, config);

    if (!check_sizes(original_image, encoded_length, config)) {
        std::cout << "Cant hide message in this image" << std::endl;
        exit(1);
    }

    uint64_t seed = 0;
    if (is_random(config)) {
        seed = time(nullptr);
    }
    if (config.VERBOSE) {
        printf("Seed: %lld\n", seed);
    }

    message_add_header(message, config, seed);
    cv::Mat output_image = original_image.clone();
    uint32_t message_offset = 0;

    write_header(output_image, message, message_offset, config);

    uint8_t bits = get_bits(config);
    int8_t channel_index = get_channel_index(config);

    if (config.VERBOSE) {
        for (uint8_t msg: message) {
            printf("(%d %c) ", msg, msg);
        }
        printf("\n");
    }

    if (is_random(config)) {
        uint32_t pixel_pos = 3;
        write_seed(output_image, message, message_offset, config, pixel_pos);
        uint32_t end = message.size() * 8;

        srand(seed);
        std::vector<uint32_t> random_positions(0);

        while (message_offset < end) {
            cv::Vec3b &pixel = get_pixel_random(output_image, random_positions, config);
            cv::Vec3b old_pixel = pixel;
            write_in_pixel(pixel, message, message_offset, end, channel_index, bits, config);

            if (config.VERBOSE)
                printf("Pixel %d: [%d, %d, %d] -> [%d, %d, %d]\n", random_positions.back(), old_pixel[0], old_pixel[1],
                       old_pixel[2],
                       pixel[0], pixel[1], pixel[2]);
        }
    } else {
        uint32_t end = message.size() * 8;
        uint32_t pixel_pos = 3;

        if (config.VERBOSE)
            printf("Start message\n");

        while (message_offset < end) {
            cv::Vec3b &pixel = get_pixel(output_image, pixel_pos);
            cv::Vec3b old_pixel = pixel;
            write_in_pixel(pixel, message, message_offset, end, channel_index, bits, config);

            if (config.VERBOSE)
                printf("Pixel %d: [%d, %d, %d] -> [%d, %d, %d]\n", pixel_pos - 1, old_pixel[0], old_pixel[1],
                       old_pixel[2],
                       pixel[0], pixel[1], pixel[2]);
        }

        if (config.VERBOSE)
            printf("End message\n");
    }

    return output_image;
}

void write_header(cv::Mat &image, const std::vector<uint8_t> &message, uint32_t &offset, const Configuration &config) {
    uint8_t bits = 1;
    int8_t channel_index = -1;
    uint32_t end = 8;
    uint32_t pixel_pos = 0;

    if (config.VERBOSE) {
        printf("Props %d\n", message[0]);
        printf("Start header\n");
    }

    while (offset < end) {
        cv::Vec3b &pixel = get_pixel(image, pixel_pos);
        cv::Vec3b old_pixel = pixel;
        write_in_pixel(pixel, message, offset, end, channel_index, bits, config);
        if (config.VERBOSE)
            printf("Pixel %d: [%d, %d, %d] -> [%d, %d, %d]\n", pixel_pos - 1, old_pixel[0], old_pixel[1], old_pixel[2],
                   pixel[0], pixel[1], pixel[2]);
        set_pixel(image, pixel_pos, pixel);
    }
    if (config.VERBOSE)
        printf("End header\n");
}

void write_seed(cv::Mat &image, const std::vector<uint8_t> &message, uint32_t &offset, const Configuration &config,
                uint32_t &pixel_pos) {
    uint8_t bits = get_bits(config);
    int8_t channel_index = get_channel_index(config);

    uint32_t end = offset + 8 * 8;

    if (config.VERBOSE)
        printf("Write seed start\n");

    while (offset < end) {
        cv::Vec3b &pixel = get_pixel(image, pixel_pos);
        cv::Vec3b old_pixel = pixel;

        if (config.VERBOSE)
            printf("Pixel %d: [%d, %d, %d] -> [%d, %d, %d]\n", pixel_pos - 1, old_pixel[0], old_pixel[1], old_pixel[2],
                   pixel[0], pixel[1], pixel[2]);

        write_in_pixel(pixel, message, offset, end, channel_index, bits, config);
    }

    if (config.VERBOSE)
        printf("Write seed end\n");
}

void write_in_pixel(cv::Vec3b &pixel, const std::vector<uint8_t> &message, uint32_t &offset, uint32_t end,
                    const int8_t channel_index, const uint8_t &bits, const Configuration &config) {
    if (channel_index == -1) {
        for (int ch = 0; ch < 3; ++ch) {
            for (int bit_i = 0; bit_i < bits; ++bit_i) {
                bool bit = get_message_bit(message, offset);
                if (config.VERBOSE)
                    printf("%d ", bit);
                pixel[ch] ^= (-bit ^ pixel[ch]) & (1 << bit_i);
                if (offset >= end) {
                    if (config.VERBOSE)
                        printf("\n");
                    return;
                }
            }
        }
    } else {
        for (int bit_i = 0; bit_i < bits; ++bit_i) {
            bool bit = get_message_bit(message, offset);

            if (config.VERBOSE)
                printf("%d ", bit);

            pixel[channel_index] ^= (-bit ^ pixel[channel_index]) & (1 << bit_i);
            if (offset >= end) {
                if (config.VERBOSE)
                    printf("\n");
                return;
            }
        }
    }

    if (config.VERBOSE)
        printf("\n");
}
