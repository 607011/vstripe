/*
        QTFFmpegWrapper - QT FFmpeg Wrapper Class
        Copyright (C) 2009,2010: Daniel Roggen, droggen@gmail.com
        Changes by Oliver Lau <oliver@ersatzworld.net>

        All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <QDebug>
#include <QObject>
#include <QMessageBox>

#include "videodecoder.h"
#include <limits.h>
#include <stdint.h>
#include "ffmpeg.h"


/**
   \brief Constructor - opens a video on later openFile call
**/
VideoDecoder::VideoDecoder()
{
    InitVars();
    initCodec();
}
/**
   \brief Constructor - opens directly a video
**/
VideoDecoder::VideoDecoder(QString file)
{
    InitVars();
    initCodec();

    ok = openFile(file.toStdString().c_str());
}

VideoDecoder::~VideoDecoder()
{
    close();
}


void VideoDecoder::InitVars()
{
    ok = false;
    pFormatCtx = 0;
    pCodecCtx = 0;
    pCodec = 0;
    pFrame = 0;
    pFrameRGB = 0;
    buffer = 0;
    img_convert_ctx = 0;
}


void VideoDecoder::close()
{
    if (!ok)
        return;
    if (buffer)
        delete [] buffer;
    if (pFrame)
        av_free(pFrame);
    if (pFrameRGB)
        av_free(pFrameRGB);
    if (pCodecCtx)
        avcodec_close(pCodecCtx);
    if (pFormatCtx)
        av_close_input_file(pFormatCtx);
    InitVars();
}


bool VideoDecoder::initCodec()
{
    ffmpeg::avcodec_init();
    ffmpeg::avcodec_register_all();
    ffmpeg::av_register_all();
//    qDebug() << QObject::tr("License: %1").arg(ffmpeg::avformat_license());
//    qDebug() << QObject::tr("AVCodec version %1").arg(ffmpeg::avformat_version());
//    qDebug() << QObject::tr("AVFormat configuration: %1").arg(ffmpeg::avformat_configuration());
    return true;
}

bool VideoDecoder::openFile(const char* filename)
{
    close();
    LastLastFrameTime = INT_MIN; // Last last must be small to handle the seek well
    LastFrameTime = 0;
    LastLastFrameNumber = INT_MIN;
    LastFrameNumber = 0;
    DesiredFrameTime = DesiredFrameNumber = 0;
    LastFrameOk = false;
    if (av_open_input_file(&pFormatCtx, filename, NULL, 0, NULL) != 0)
        return false; // Couldn't open file
    if (av_find_stream_info(pFormatCtx)<0)
        return false; // Couldn't find stream information
    videoStream = -1;
    for (unsigned i = 0; i < pFormatCtx->nb_streams; ++i) {
        if (pFormatCtx->streams[i]->codec->codec_type == ffmpeg::CODEC_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    }
    if (videoStream == -1)
        return false; // Didn't find a video stream
    pCodecCtx = pFormatCtx->streams[videoStream]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL)
        return false; // Codec not found
    //if (pCodecCtx->time_base.num > 1000 && pCodecCtx->time_base.den == 1)
    //    pCodecCtx->time_base.den = 1000;
    if (avcodec_open(pCodecCtx, pCodec) < 0)
        return false; // Could not open codec
    if (QString(pFormatCtx->iformat->long_name) == "QuickTime/MPEG-4/Motion JPEG 2000 format") {
        mDefaultSkip = 1000;
    }
    else if (QString(pFormatCtx->iformat->long_name) == "AVI format") {
        mDefaultSkip = 1;
    }
    else if (QString(pFormatCtx->iformat->long_name) == "Matroska/WebM file format") {
        mDefaultSkip = 40;
    }
    else if (QString(pFormatCtx->iformat->long_name) == "FLV format") {
        mDefaultSkip = 40;
    }
    else if (QString(pFormatCtx->iformat->long_name) == "ASF format") {
        mDefaultSkip = 36;
    }
    else {
        mDefaultSkip = 1;
    }
    qDebug() << "mDefaultSkip = " << mDefaultSkip;
    pFrame = ffmpeg::avcodec_alloc_frame();
    pFrameRGB = ffmpeg::avcodec_alloc_frame();
    if (pFrameRGB == NULL)
        return false;
    numBytes = ffmpeg::avpicture_get_size(ffmpeg::PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
    buffer = new uint8_t[numBytes];
    avpicture_fill((ffmpeg::AVPicture *)pFrameRGB, buffer, ffmpeg::PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
    ok = true;
    return true;
}


bool VideoDecoder::isOk() const
{
    return ok;
}


QSize VideoDecoder::frameSize(void) const
{
    return QSize(pCodecCtx->width, pCodecCtx->height);
}


/**
   Decodes the video stream until the first frame with number larger or equal than 'after' is found.

   Returns:
   - true if a frame is found, false otherwise.
   - the image as a QImage if img is non-null
   - time frame time, if frametime is non-null
   - the frame number, if framenumber is non-null

   All times are in milliseconds.
**/
bool VideoDecoder::decodeSeekFrame(int after)
{
    if (!ok)
        return false;

    if (after != -1 && (LastFrameOk && after >= LastLastFrameNumber && after <= LastFrameNumber)) {
        // Compute desired frame time
        ffmpeg::AVRational millisecondbase = {1, 1000};
        DesiredFrameTime = ffmpeg::av_rescale_q(after,pFormatCtx->streams[videoStream]->time_base,millisecondbase);
        return true;
    }

    bool done = false;
    while (!done) {
        if (av_read_frame(pFormatCtx, &packet) < 0)
            return false;
        if (packet.stream_index == videoStream) {
            int frameFinished;
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

            if (frameFinished) {
                ffmpeg::AVRational millisecondbase = {1, 1000};
                int f = packet.dts;
                int t = ffmpeg::av_rescale_q(packet.dts,pFormatCtx->streams[videoStream]->time_base, millisecondbase);
                if (!LastFrameOk) {
                    LastFrameOk = true;
                    LastLastFrameTime = LastFrameTime = t;
                    LastLastFrameNumber = LastFrameNumber = f;
                }
                else {
                    // If we decoded 2 frames in a row, the last times are okay
                    LastLastFrameTime = LastFrameTime;
                    LastLastFrameNumber = LastFrameNumber;
                    LastFrameTime = t;
                    LastFrameNumber = f;
                }
                if (after == -1 || LastFrameNumber >= after) {
                    // Convert the image format (init the context the first time)
                    int w = pCodecCtx->width;
                    int h = pCodecCtx->height;
                    img_convert_ctx = ffmpeg::sws_getCachedContext(img_convert_ctx,w, h, pCodecCtx->pix_fmt, w, h, ffmpeg::PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
                    if (img_convert_ctx == NULL) {
                        qDebug() << QObject::tr("Cannot initialize the conversion context!");
                        return false;
                    }
                    ffmpeg::sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
                    LastFrame = QImage(w, h, QImage::Format_RGB888);
                    for (int y = 0; y < h; ++y)
                        memcpy(LastFrame.scanLine(y), pFrameRGB->data[0]+y*pFrameRGB->linesize[0], w*3);
                    DesiredFrameTime = ffmpeg::av_rescale_q(after, pFormatCtx->streams[videoStream]->time_base, millisecondbase);
                    LastFrameOk = true;
                    done = true;
                }
            }
        }
        av_free_packet(&packet);
    }
    return done;
}

/**
   \brief Decodes the next frame in the video stream
**/
bool VideoDecoder::seekNextFrame(int skip)
{
    if (skip == 0)
        skip = mDefaultSkip;
    bool ret = decodeSeekFrame(DesiredFrameNumber + skip);
    if (ret)
        DesiredFrameNumber += skip; // Only updates the DesiredFrameNumber if we were successful in getting that frame
    else
        LastFrameOk = false; // We didn't find the next frame (e.g. seek out of range) - mark we don't know where we are.
    return ret;
}

/**
  \brief Seek to millisecond
**/
bool VideoDecoder::seekMs(int tsms)
{
    if (!ok)
        return false;
    DesiredFrameNumber = ffmpeg::av_rescale(tsms, pFormatCtx->streams[videoStream]->time_base.den, pFormatCtx->streams[videoStream]->time_base.num);
    DesiredFrameNumber /= 1000;
    return seekFrame(DesiredFrameNumber);
}


/**
  \brief Seek to frame
**/
bool VideoDecoder::seekFrame(qint64 frame)
{
    if (!ok)
        return false;
    if (!LastFrameOk || (LastFrameOk && (frame <= LastLastFrameNumber || frame > LastFrameNumber)))
    {
        if (ffmpeg::avformat_seek_file(pFormatCtx, videoStream, 0, frame, frame, AVSEEK_FLAG_FRAME) < 0)
            return false;
        avcodec_flush_buffers(pCodecCtx);
        DesiredFrameNumber = frame;
        LastFrameOk = false;
    }
    return decodeSeekFrame(frame);
}


bool VideoDecoder::getFrame(QImage& img, int* effectiveframenumber, int* effectiveframetime, int* desiredframenumber, int* desiredframetime) const
{
    img = LastFrame;
    if (effectiveframenumber)
        *effectiveframenumber = LastFrameNumber;
    if (effectiveframetime)
        *effectiveframetime = LastFrameTime;
    if (desiredframenumber)
        *desiredframenumber = DesiredFrameNumber;
    if (desiredframetime)
        *desiredframetime = DesiredFrameTime;
    return LastFrameOk;
}


int VideoDecoder::getVideoLengthMs()
{
    if (!isOk())
        return -1;
    int secs = pFormatCtx->duration / AV_TIME_BASE;
    int us = pFormatCtx->duration % AV_TIME_BASE;
    int l = secs * 1000 + us / 1000;
    return l;
}

