#include <iostream>

#include "sample_video_define.hpp"

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}

int main(void) {
    std::cout << LIBAVFORMAT_VERSION_MAJOR << " "
              << LIBAVFORMAT_VERSION_MINOR << " "
              << LIBAVFORMAT_VERSION_MICRO << std::endl;

    AVFormatContext* formatCtx = NULL;
    if (avformat_open_input(&formatCtx, SAMPLE_VIDEO_PATH, NULL, NULL) < 0) {
        std::cout << "avformat_open_input() failed" << std::endl;
        return -1;
    }

    av_dump_format(formatCtx, 0, SAMPLE_VIDEO_PATH, 0);
    
    avformat_close_input(&formatCtx);
    
    return 0;
}