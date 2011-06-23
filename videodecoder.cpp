/*
        QTFFmpegWrapper - QT FFmpeg Wrapper Class
        Copyright (C) 2009,2010: Daniel Roggen, droggen@gmail.com
        Changes by Oliver Lau <oliver@von-und-fuer-lau.de>

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


VideoDecoder::VideoDecoder(void)
{
    initVars();
    initCodec();
}


VideoDecoder::VideoDecoder(QString file)
{
    initVars();
    initCodec();
    mOk = open(file.toLatin1().constData());
}

VideoDecoder::~VideoDecoder()
{
    close();
}


void VideoDecoder::initVars()
{
    mOk = false;
    mFormatCtx = NULL;
    mCodecCtx = NULL;
    mCodec = NULL;
    mFrame = NULL;
    mFrameRGB = NULL;
    mBuf = NULL;
    mImgConvCtx = NULL;
}


void VideoDecoder::close()
{
    if (!isOk())
        return;
    if (mBuf)
        delete [] mBuf;
    if (mFrame)
        ffmpeg::av_free(mFrame);
    if (mFrameRGB)
        ffmpeg::av_free(mFrameRGB);
    if (mCodecCtx)
        ffmpeg::avcodec_close(mCodecCtx);
    if (mFormatCtx)
        ffmpeg::av_close_input_file(mFormatCtx);
    initVars();
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


bool VideoDecoder::open(int)
{
    return false;
}


bool VideoDecoder::open(const char* filename)
{
    close();
    mLastLastFrameTime = INT_MIN; // Last last must be small to handle the seek well
    mLastFrameTime = 0;
    mLastLastFrameNumber = INT_MIN;
    mLastFrameNumber = 0;
    mDesiredFrameTime = 0;
    mDesiredFrameNumber = 0;
    mLastFrameOk = false;
    if (ffmpeg::av_open_input_file(&mFormatCtx, filename, NULL, 0, NULL) != 0)
        return false; // Couldn't open file
    if (ffmpeg::av_find_stream_info(mFormatCtx)<0)
        return false; // Couldn't find stream information
    mVideoStream = -1;
    for (unsigned int i = 0; i < mFormatCtx->nb_streams; ++i) {
        if (mFormatCtx->streams[i]->codec->codec_type == ffmpeg::AVMEDIA_TYPE_VIDEO) {
            mVideoStream = i;
            break;
        }
    }
    if (mVideoStream == -1)
        return false; // Didn't find a video stream
    mCodecCtx = mFormatCtx->streams[mVideoStream]->codec;
    if (mCodecCtx == NULL)
        return false;
    mCodec = ffmpeg::avcodec_find_decoder(mCodecCtx->codec_id);
    if (mCodec == NULL)
        return false; // Codec not found
    //if (pCodecCtx->time_base.num > 1000 && pCodecCtx->time_base.den == 1)
    //    pCodecCtx->time_base.den = 1000;
    if (avcodec_open(mCodecCtx, mCodec) < 0)
        return false; // Could not open codec
    if (QString(mFormatCtx->iformat->long_name) == "QuickTime/MPEG-4/Motion JPEG 2000 format") {
        mDefaultSkip = 1000;
    }
    else if (QString(mFormatCtx->iformat->long_name) == "AVI format") {
        mDefaultSkip = 1;
    }
    else if (QString(mFormatCtx->iformat->long_name) == "Matroska/WebM file format") {
        mDefaultSkip = 40;
    }
    else if (QString(mFormatCtx->iformat->long_name) == "FLV format") {
        mDefaultSkip = 40;
    }
    else if (QString(mFormatCtx->iformat->long_name) == "ASF format") {
        mDefaultSkip = 36;
    }
    else {
        mDefaultSkip = 1;
    }
    mFrame = ffmpeg::avcodec_alloc_frame();
    mFrameRGB = ffmpeg::avcodec_alloc_frame();
    if (mFrameRGB == NULL || mFrame == NULL)
        return false;
    mByteCount = ffmpeg::avpicture_get_size(ffmpeg::PIX_FMT_RGB24, mCodecCtx->width, mCodecCtx->height);
    mBuf = new quint8[mByteCount];
    avpicture_fill((ffmpeg::AVPicture *)mFrameRGB, mBuf, ffmpeg::PIX_FMT_RGB24, mCodecCtx->width, mCodecCtx->height);
    mOk = true;
    return true;
}


QSize VideoDecoder::frameSize(void) const
{
    return QSize(mCodecCtx->width, mCodecCtx->height);
}


bool VideoDecoder::decodeSeekFrame(int after)
{
    if (!isOk())
        return false;

    if (after != -1 && (mLastFrameOk && after >= mLastLastFrameNumber && after <= mLastFrameNumber)) {
        // Compute desired frame time
        ffmpeg::AVRational millisecondbase = {1, 1000};
        mDesiredFrameTime = ffmpeg::av_rescale_q(after, mFormatCtx->streams[mVideoStream]->time_base, millisecondbase);
        return true;
    }

    bool done = false;
    while (!done) {
        if (ffmpeg::av_read_frame(mFormatCtx, &mPacket) < 0)
            return false;
        if (mPacket.stream_index == mVideoStream) {
            int frameFinished;
            ffmpeg::avcodec_decode_video2(mCodecCtx, mFrame, &frameFinished, &mPacket);
            if (frameFinished) {
                ffmpeg::AVRational millisecondbase = {1, 1000};
                const int f = mPacket.dts;
                const int t = ffmpeg::av_rescale_q(mPacket.dts, mFormatCtx->streams[mVideoStream]->time_base, millisecondbase);
                if (!mLastFrameOk) {
                    mLastFrameOk = true;
                    mLastLastFrameTime = mLastFrameTime = t;
                    mLastLastFrameNumber = mLastFrameNumber = f;
                }
                else {
                    // If we decoded 2 frames in a row, the last times are okay
                    mLastLastFrameTime = mLastFrameTime;
                    mLastLastFrameNumber = mLastFrameNumber;
                    mLastFrameTime = t;
                    mLastFrameNumber = f;
                }
                if (after == -1 || mLastFrameNumber >= after) {
                    // Convert the image format (init the context the first time)
                    const int w = mCodecCtx->width;
                    const int h = mCodecCtx->height;
                    mImgConvCtx = ffmpeg::sws_getCachedContext(mImgConvCtx, w, h, mCodecCtx->pix_fmt, w, h, ffmpeg::PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
                    if (mImgConvCtx == NULL) {
                        qDebug() << QObject::tr("Cannot initialize the conversion context!");
                        return false;
                    }
                    ffmpeg::sws_scale(mImgConvCtx, mFrame->data, mFrame->linesize, 0, mCodecCtx->height, mFrameRGB->data, mFrameRGB->linesize);
                    mLastFrame = QImage(w, h, QImage::Format_RGB888);
                    for (int y = 0; y < h; ++y)
                        memcpy(mLastFrame.scanLine(y), mFrameRGB->data[0]+y*mFrameRGB->linesize[0], w*3);
                    mDesiredFrameTime = ffmpeg::av_rescale_q(after, mFormatCtx->streams[mVideoStream]->time_base, millisecondbase);
                    mLastFrameOk = true;
                    done = true;
                }
            }
        }
        ffmpeg::av_free_packet(&mPacket);
    }
    return done;
}


bool VideoDecoder::seekNextFrame(int skip)
{
    skip *= mDefaultSkip;
    bool ret = decodeSeekFrame(mDesiredFrameNumber + skip);
    if (ret)
        mDesiredFrameNumber += skip; // Only updates the DesiredFrameNumber if we were successful in getting that frame
    else
        mLastFrameOk = false; // We didn't find the next frame (e.g. seek out of range) - mark we don't know where we are.
    return ret;
}


bool VideoDecoder::seekMs(int tsms)
{
    if (!isOk())
        return false;
    mDesiredFrameNumber = ffmpeg::av_rescale(tsms, mFormatCtx->streams[mVideoStream]->time_base.den, mFormatCtx->streams[mVideoStream]->time_base.num);
    mDesiredFrameNumber /= 1000;
    return seekFrame(mDesiredFrameNumber);
}


bool VideoDecoder::seekFrame(qint64 frame)
{
    if (!isOk())
        return false;
    if (!mLastFrameOk || (mLastFrameOk && (frame <= mLastLastFrameNumber || frame > mLastFrameNumber)))
    {
        if (ffmpeg::avformat_seek_file(mFormatCtx, mVideoStream, 0, frame, frame, AVSEEK_FLAG_FRAME) < 0)
            return false;
        avcodec_flush_buffers(mCodecCtx);
        mDesiredFrameNumber = frame;
        mLastFrameOk = false;
    }
    return decodeSeekFrame(frame);
}


bool VideoDecoder::getFrame(QImage& img, int* effectiveframenumber, int* effectiveframetime, int* desiredframenumber, int* desiredframetime) const
{
    img = mLastFrame;
    if (effectiveframenumber)
        *effectiveframenumber = mLastFrameNumber;
    if (effectiveframetime)
        *effectiveframetime = mLastFrameTime;
    if (desiredframenumber)
        *desiredframenumber = mDesiredFrameNumber;
    if (desiredframetime)
        *desiredframetime = mDesiredFrameTime;
    return mLastFrameOk;
}


int VideoDecoder::getVideoLengthMs(void)
{
    if (!isOk())
        return -1;
    int secs = mFormatCtx->duration / AV_TIME_BASE;
    int us = mFormatCtx->duration % AV_TIME_BASE;
    int l = secs * 1000 + us / 1000;
    return l;
}
