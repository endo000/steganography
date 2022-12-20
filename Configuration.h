//
// Created by nsakn on 12/15/2022.
//

#ifndef STEGANOGRAFIA_CONFIGURATION_H
#define STEGANOGRAFIA_CONFIGURATION_H

#include <string>
#include <bitset>

// properties:
// 0 bit - hide(false)/unhide(true)
// 1-3 bits - how many bits to hide in one pixel (max. 4)
// 4-6 bits - r(1)/g(2)/b(4)
// 7 bit - random
struct Configuration {
    std::string image;
    std::string message;
    std::string output;
    std::bitset<8> properties;
    bool VERBOSE;
};
#endif //STEGANOGRAFIA_CONFIGURATION_H
