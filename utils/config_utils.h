//
// Created by nsakn on 12/15/2022.
//

#ifndef STEGANOGRAFIA_CONFIG_UTILS_H
#define STEGANOGRAFIA_CONFIG_UTILS_H

#include "../Configuration.h"

Configuration read_flags(int argc, char **argv);
bool is_hide(const Configuration &config);
bool is_random(const Configuration &config);
bool is_all_channels(const Configuration &config);
uint8_t get_bits(const Configuration &config);
int8_t get_channel_index(const Configuration &config);

#endif //STEGANOGRAFIA_CONFIG_UTILS_H
