//
// Created by nsakn on 12/21/2022.
//

#ifndef STEGANOGRAFIA_OPENMPICONFIGURATION_H
#define STEGANOGRAFIA_OPENMPICONFIGURATION_H

#include <string>
#include <vector>
#include <bitset>

struct OpenMpiConfiguration {
    bool hide;
    bool VERBOSE;
    std::vector<std::string> images;
    std::string images_dir;
    std::string book;
    std::string output_dir;
    std::bitset<8> properties;
};
#endif //STEGANOGRAFIA_OPENMPICONFIGURATION_H
