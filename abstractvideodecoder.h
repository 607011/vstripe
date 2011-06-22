#ifndef __IABSTRACTVIDEODECODER_H_
#define __IABSTRACTVIDEODECODER_H_

#include <QtGui/QImage>

class IAbstractVideoDecoder : public QObject
{
public:
    IAbstractVideoDecoder(QObject* parent = NULL) : QObject(parent) {}
    virtual bool openFile(const char* fileName) = 0;
    virtual bool seekFrame(qint64 frame) = 0;
    virtual bool seekNextFrame(int skip) = 0;
    virtual bool getFrame(QImage& img, int* effectiveframenumber, int* effectiveframetime, int* desiredframenumber, int* desiredframetime) const = 0;
};

#endif // __IABSTRACTVIDEODECODER_H_
