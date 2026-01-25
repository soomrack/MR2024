#include "VideoReceiver.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

VideoReceiver::VideoReceiver(const QString& url, QObject* parent)
    : QThread(parent), streamUrl(url), running(true) {}

void VideoReceiver::stop() {
    running = false;
}

void VideoReceiver::run() {
    avformat_network_init();

    AVFormatContext* fmt = nullptr;
    if (avformat_open_input(&fmt, streamUrl.toStdString().c_str(), nullptr, nullptr) < 0)
        return;

    avformat_find_stream_info(fmt, nullptr);

    int videoStream = -1;
    for (unsigned i = 0; i < fmt->nb_streams; i++) {
        if (fmt->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    }
    if (videoStream < 0) return;

    AVCodec* codec = avcodec_find_decoder(
        fmt->streams[videoStream]->codecpar->codec_id);

    AVCodecContext* ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(ctx, fmt->streams[videoStream]->codecpar);
    avcodec_open2(ctx, codec, nullptr);

    AVFrame* frame = av_frame_alloc();
    AVPacket packet;

    SwsContext* sws = sws_getContext(
        ctx->width, ctx->height, ctx->pix_fmt,
        ctx->width, ctx->height, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, nullptr, nullptr, nullptr);

    uint8_t* rgbBuffer = new uint8_t[ctx->width * ctx->height * 3];

    while (running && av_read_frame(fmt, &packet) >= 0) {
        if (packet.stream_index == videoStream) {
            avcodec_send_packet(ctx, &packet);
            if (avcodec_receive_frame(ctx, frame) == 0) {

                uint8_t* dest[4] = { rgbBuffer, nullptr, nullptr, nullptr };
                int destLinesize[4] = { ctx->width * 3, 0, 0, 0 };

                sws_scale(sws,
                          frame->data, frame->linesize,
                          0, ctx->height,
                          dest, destLinesize);

                QImage img(rgbBuffer,
                           ctx->width,
                           ctx->height,
                           QImage::Format_RGB888);

                emit frameReady(img.copy());
            }
        }
        av_packet_unref(&packet);
    }

    delete[] rgbBuffer;
    av_frame_free(&frame);
    avcodec_free_context(&ctx);
    avformat_close_input(&fmt);
}
