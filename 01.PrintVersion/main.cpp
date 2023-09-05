#include <iostream>

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}

int main(void) {
    std::cout << LIBAVFORMAT_VERSION_MAJOR << " "
              << LIBAVFORMAT_VERSION_MINOR << " "
              << LIBAVFORMAT_VERSION_MICRO << std::endl;

    AVFormatContext* formatCtx = NULL;
    if (avformat_open_input(&formatCtx, "", NULL, NULL) < 0) {
        std::cout << "avformat_open_input() failed" << std::endl;
        return -1;
    }
    
    return 0;
}