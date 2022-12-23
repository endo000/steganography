//
// Created by nsakn on 12/23/2022.
//

#ifndef STEGANOGRAFIA_RANKDATA_H
#define STEGANOGRAFIA_RANKDATA_H

#include <string>
#include <vector>

struct RankData {
    std::string image_name;
    std::vector<uint8_t> text;
};
#endif //STEGANOGRAFIA_RANKDATA_H
