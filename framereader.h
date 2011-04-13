#ifndef FRAMEREADER_H
#define FRAMEREADER_H


extern "C" {
#include <avcodec.h>
#include <avformat.h>
}
#include <stdio.h>


extern bool GetNextFrame(AVFormatContext *pFormatCtx, AVCodecContext *pCodecCtx, int videoStream, AVFrame *pFrame);

#endif // FRAMEREADER_H
