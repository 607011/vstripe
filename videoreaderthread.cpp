/*
 * Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * $Id$
 */

#include <QDebug>
#include <QMutexLocker>

#include "videoreaderthread.h"

VideoReaderThread::VideoReaderThread(VideoWidget* videoWidget, QObject* parent)
    : QThread(parent), mVideoWidget(videoWidget)
{
    abort = false;
}


VideoReaderThread::~VideoReaderThread()
{
    stopReading();
}


void VideoReaderThread::startReading(const QString& videoFileName)
{
    mFileName = videoFileName;
    abort = false;
    start();
}


void VideoReaderThread::stopReading(void)
{
    abort = true;
    wait();
}


void VideoReaderThread::run(void)
{
    char* const filename = new char[mFileName.size()+1];
    strcpy(filename, mFileName.toLocal8Bit().constData());

    // Open video file
    AVFormatContext* pFormatCtx;
    if (av_open_input_file(&pFormatCtx, filename, NULL, 0, NULL) != 0) {
        qDebug() << tr("Couldn't open file '%1'.").arg(filename);
        exit(-1);
    }

    // Retrieve stream information
    if (av_find_stream_info(pFormatCtx) < 0) {
        qDebug() << tr("Couldn't find stream information");
        exit(-1);
    }

    // Find the first video stream
    int videoStream = -1;
    for (unsigned int i = 0; i < pFormatCtx->nb_streams; ++i) {
        if (pFormatCtx->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    }
    if (videoStream == -1) {
        qDebug() << tr("Didn't find a video stream");
        exit(-1);
    }

    // Get a pointer to the codec context for the video stream
    AVCodecContext* pCodecCtx = pFormatCtx->streams[videoStream]->codec;
    mVideoWidget->setFrameSize(pCodecCtx->width, pCodecCtx->height);

    // Find the decoder for the video stream
    AVCodec* pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL) {
        qDebug() << tr("Codec %1 not found").arg(pCodecCtx->codec_id);
        exit(-1);
    }

    // Inform the codec that we can handle truncated bitstreams -- i.e.,
    // bitstreams where frame boundaries can fall in the middle of packets
    if (pCodec->capabilities & CODEC_CAP_TRUNCATED)
        pCodecCtx->flags |= CODEC_FLAG_TRUNCATED;

    if (avcodec_open(pCodecCtx, pCodec) < 0) {
        qDebug() << "Could not open codec";
        exit(-1);
    }

    // Allocate video frame
    AVFrame* pFrame = avcodec_alloc_frame();

    // Allocate an AVFrame structure
    AVFrame* pFrameRGB = avcodec_alloc_frame();
    if (pFrameRGB == NULL)
        exit(-1);

    // Determine required buffer size and allocate buffer
    int numBytes = avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
    uint8_t* buffer = new uint8_t[numBytes];

    // Assign appropriate parts of buffer to image planes in pFrameRGB
    avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);

    int frameFinished;
    AVPacket avpkt;
    av_init_packet(&avpkt);

    int frameCount = 0;
    SwsContext* img_convert_ctx = NULL;
    while (av_read_frame(pFormatCtx, &avpkt) >= 0 && !abort) {
        if (avpkt.stream_index == videoStream) {
            avcodec_decode_video2(pCodecCtx, pFrameRGB, &frameFinished, &avpkt);
            if (frameFinished) {
                if (img_convert_ctx == NULL) {
                    int w = pCodecCtx->width;
                    int h = pCodecCtx->height;
                    img_convert_ctx = sws_getContext(w, h, pCodecCtx->pix_fmt, w, h, PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
                    if (img_convert_ctx == NULL) {
                        qDebug() << "Cannot initialize the conversion context!";
                        exit(-1);
                    }
                }
                sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
                mVideoWidget->setFrame(pFrameRGB);
                ++frameCount;
            }
        }
        av_free_packet(&avpkt);
    }

    delete [] filename;
    delete [] buffer;
    av_free(pFrameRGB);
    av_free(pFrame);
    avcodec_close(pCodecCtx);
    av_close_input_file(pFormatCtx);
}
