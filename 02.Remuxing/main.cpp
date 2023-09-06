#include <iostream>
#include <string>

#include "sample_video_define.hpp"

extern "C" {
    #include "libavutil/timestamp.h"
    #include "libavformat/avformat.h"
}

void closeInputVideo(AVFormatContext** formatContext) {
    avformat_close_input(formatContext);
}

void closeOutputVideo(AVFormatContext* formatContext) {
    if (formatContext->oformat->flags & AVFMT_NOFILE) {
        avio_closep(&formatContext->pb);
    }
    avformat_free_context(formatContext);
}

int openVideo(AVFormatContext** formatContext, std::string& inputFileName) {
    int retValue = 0;
    retValue = avformat_open_input(formatContext, inputFileName.c_str(), nullptr, nullptr);
    if (retValue < 0) {
        std::cout << "Could not open input file '" << inputFileName << "'" << std::endl;
        return retValue;
    }

    retValue = avformat_find_stream_info(*formatContext, nullptr);
    if (retValue < 0) {
        std::cout << "Failed find stream info " << std::endl;
        avformat_close_input(formatContext);
        return retValue;
    }

    return 0;
}

int openOutputVideo(AVFormatContext** formatContext, std::string& outputFileName) {
    avformat_alloc_output_context2(formatContext, nullptr, nullptr, outputFileName.c_str());
    if (*formatContext == nullptr) {
        std::cout << "Could not create output context" << std::endl;
        return AVERROR_UNKNOWN;
    }
    
    if(!((*formatContext)->oformat->flags & AVFMT_NOFILE)) {
        int retValue = avio_open(&(*formatContext)->pb, outputFileName.c_str(), AVIO_FLAG_WRITE);
        if (retValue < 0) {
            std::cout << "Could not open output file '" << outputFileName << "'" << std::endl;
            return -1;
        }
    }

    return 0;
}

int copyVideo(AVFormatContext* inputFormatContext, AVFormatContext* outputFormatContext, bool isMP4) {
    int numberOfStreams = inputFormatContext->nb_streams;
    int* streamsList = (int*)av_calloc(numberOfStreams,sizeof(int));
    if (streamsList == nullptr) {
        return ENOMEM;
    }

    int streamIndex = 0;
    for (int i = 0; i < inputFormatContext->nb_streams; i++) {
        AVCodecParameters* inputCodecParameter = inputFormatContext->streams[i]->codecpar;

        if (inputCodecParameter->codec_type != AVMEDIA_TYPE_AUDIO &&
            inputCodecParameter->codec_type != AVMEDIA_TYPE_VIDEO &&
            inputCodecParameter->codec_type != AVMEDIA_TYPE_SUBTITLE) {
                streamsList[i] = -1;
                continue;
            }
        streamsList[i] = streamIndex++;
        
        AVStream* outputStream = avformat_new_stream(outputFormatContext, nullptr);
        if (outputStream == nullptr) {
            std::cout << "Failed allocate output stream" << std::endl;
            return AVERROR_UNKNOWN;
        }

        int retValue = avcodec_parameters_copy(outputStream->codecpar, inputCodecParameter);
        if (retValue < 0) {
            std::cout << "Failed copy codec parameters" << std::endl;
            return retValue;
        }
    }

    AVDictionary* opts = nullptr;
    if (isMP4 == true) {
        av_dict_set(&opts, "movflags", "frag_keyframe+empty_moov+default_base_moof", 0);
    }

    int retValue = 0;
    retValue = avformat_write_header(outputFormatContext, &opts);
    if (retValue < 0) {
        std::cout << "Write header failed" << std::endl;
        return retValue;
    }

    while (true) {
        AVStream* inputStream = nullptr;
        AVStream* outputStream = nullptr;

        AVPacket packet;
        retValue = av_read_frame(inputFormatContext, &packet);
        if (retValue < 0) {
            av_freep(&streamsList);
            av_packet_unref(&packet);
            return retValue;
        }

        inputStream = inputFormatContext->streams[packet.stream_index];
        if (packet.stream_index >= numberOfStreams || streamsList[packet.stream_index] < 0) {
            av_freep(&streamsList);
            av_packet_unref(&packet);
            continue;
        }

        packet.stream_index = streamsList[packet.stream_index];
        outputStream = outputFormatContext->streams[packet.stream_index];
        // copy packet
        packet.pts = av_rescale_q_rnd(packet.pts, inputStream->time_base, outputStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        packet.dts = av_rescale_q_rnd(packet.dts, inputStream->time_base, outputStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        packet.duration = av_rescale_q(packet.duration, inputStream->time_base, outputStream->time_base);
        packet.pos = -1;

        retValue = av_interleaved_write_frame(outputFormatContext, &packet);
        if (retValue < 0) {
            std::cout << "Error muxing packet" << std::endl;
            av_freep(&streamsList);
            av_packet_unref(&packet);
            return -1;
        }
    }
    av_freep(&streamsList);
    return 0;
}

int main(void) {
    AVFormatContext* inputFormatContext = nullptr;
    AVFormatContext* outputFormatContext = nullptr;

    std::string inputFileName = SAMPLE_VIDEO_PATH;
    std::string outputFileName = "./sample_video.mp4";

    if (openVideo(&inputFormatContext, inputFileName) < 0) {
        return -1;
    }

    if (openOutputVideo(&outputFormatContext, outputFileName) < 0) {
        closeInputVideo(&inputFormatContext);
        return -1;
    }

    if (copyVideo(inputFormatContext, outputFormatContext, true) < 0) {
        closeInputVideo(&inputFormatContext);
        closeOutputVideo(outputFormatContext);
        return -1;
    }

    closeInputVideo(&inputFormatContext);
    closeOutputVideo(outputFormatContext);

    return 0;
}