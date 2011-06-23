/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#ifndef __IABSTRACTVIDEODECODER_H_
#define __IABSTRACTVIDEODECODER_H_

#include <QtGui/QImage>

class IAbstractVideoDecoder : public QObject
{
public:

    virtual bool open(const char* fileName) = 0;
    virtual bool open(int deviceId) = 0;
    virtual void close(void) = 0;
    virtual bool seekFrame(qint64 frame) = 0;
    virtual bool seekNextFrame(int skip) = 0;
    virtual bool getFrame(QImage& img, int* effectiveframenumber, int* effectiveframetime, int* desiredframenumber, int* desiredframetime) const = 0;
    virtual int attributes(void) = 0;

protected:
    IAbstractVideoDecoder(QObject* parent = NULL) : QObject(parent) { /* ... */ }
    enum Attributes { NONE = 0, SEEK_FRAME = 0x0001, SEEK_NEXT_FRAME = 0x0002, SEEK_TO_MS = 0x0004, OPEN_CLOSE = 0x0010 };

};

#endif // __IABSTRACTVIDEODECODER_H_
