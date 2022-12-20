#include <vector>
#include <cstring>
#include <iostream>
#include <algorithm>
#include "../Configuration.h"

Configuration read_flags(int argc, char **argv) {
    Configuration config;
    config.VERBOSE = false;

    std::vector<bool> required_flags(4, false);
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0) {
            if (config.VERBOSE)
                std::cout << "Hide" << std::endl;

            required_flags[0] = true;

            config.properties[0] = false;
        } else if (strcmp(argv[i], "-u") == 0) {
            if (config.VERBOSE)
                std::cout << "Unhide" << std::endl;

            required_flags[0] = true;
            required_flags[2] = true;

            config.properties[0] = true;
        } else if (strcmp(argv[i], "-i") == 0) {
            i++;
            if (config.VERBOSE)
                std::cout << "Image: " << argv[i] << std::endl;

            required_flags[1] = true;

            config.image = argv[i];
        } else if (strcmp(argv[i], "-m") == 0) {
            i++;
            if (config.VERBOSE)
                std::cout << "Message: " << argv[i] << std::endl;

            required_flags[2] = true;

            config.message = argv[i];
        } else if (strcmp(argv[i], "-o") == 0) {
            i++;
            if (config.VERBOSE)
                std::cout << "Output: " << argv[i] << std::endl;

            required_flags[3] = true;

            config.output = argv[i];
        } else if (strcmp(argv[i], "-r") == 0) {
            if (config.VERBOSE)
                std::cout << "Only red" << std::endl;

            config.properties[4] = true;
            config.properties[5] = false;
            config.properties[6] = false;
        } else if (strcmp(argv[i], "-g") == 0) {
            if (config.VERBOSE)
                std::cout << "Only green" << std::endl;

            config.properties[4] = false;
            config.properties[5] = true;
            config.properties[6] = false;
        } else if (strcmp(argv[i], "-b") == 0) {
            if (config.VERBOSE)
                std::cout << "Only blue" << std::endl;

            config.properties[4] = false;
            config.properties[5] = false;
            config.properties[6] = true;
        } else if (strcmp(argv[i], "--bits") == 0) {
            i++;
            if (config.VERBOSE)
                std::cout << "Bits: " << argv[i] << std::endl;

            int bits = std::stoi(argv[i]);
            if (0 > bits || bits > 4) {
                std::cout << "Wrong bits number" << std::endl;
                exit(1);
            }

            for (int j = 0; j < 3; ++j) {
                config.properties[1 + j] = (bits >> j) & 1;
            }
        } else if (strcmp(argv[i], "--random") == 0) {
            if (config.VERBOSE)
                std::cout << "Random" << std::endl;

            config.properties[7] = true;
        }
    }
    if (!std::all_of(required_flags.begin(), required_flags.end(), [](bool v) { return v; })) {
        std::cout << "Not all required Configuration passed" << std::endl;
        exit(1);
    }

    return config;
}

bool is_hide(const Configuration &config) {
    return !config.properties[0];
}

bool is_random(const Configuration &config) {
    return config.properties[7];
}

bool is_all_channels(const Configuration &config) {
    if (config.properties[4] || config.properties[5] || config.properties[6]) {
        return false;
    }

    return true;
}

uint8_t get_bits(const Configuration &config) {
    uint8_t bits = 0;
    bits |= config.properties[1];
    bits |= config.properties[2] << 1;
    bits |= config.properties[3] << 2;

    if (bits == 0) {
        return 1;
    }

    return bits;
}

int8_t get_channel_index(const Configuration &config) {
    // -1 == all channels
    int8_t index = -1;
    for (int8_t i = 0; i < 3; ++i) {
        if (config.properties[i + 4]) {
            index = i;
        }
    }

    // Switch channel index because CV2 is usually BGR
    if (index == 0) {
        index = 2;
    } else if (index == 2) {
        index = 0;
    }

    return index;
}

