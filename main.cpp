#include <opencv2/opencv.hpp>
#include <vector>
#include "Configuration.h"
#include "utils/utils.h"
#include "utils/config_utils.h"
#include "steganography/steganography.h"
#include "steganalysis/steganalysis.h"

int main(int argc, char **argv) {
    Configuration config = read_flags(argc, argv);

    cv::Mat image = read_image(config);

    std::vector<uint8_t> message;
    if (is_hide(config)) {
        message = read_message_file(config);
        cv::Mat hided_image = steganography(image, message, config);
        write_image(config, hided_image);
    } else {
        message = steganalysis(image, config);
        message_write_file(message, config);
    }

    return 0;
}



