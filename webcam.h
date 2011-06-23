#ifndef __WEBCAM_H_
#define __WEBCAM_H_

#include <QtCore>
#include "opencv2/opencv.hpp"
#include "abstractvideodecoder.h"



class Webcam : public IAbstractVideoDecoder
{
    Q_OBJECT
public:
    Webcam(QObject* parent = NULL);
    virtual ~Webcam();
    virtual bool open(const char*);
    virtual bool open(int deviceId);
    virtual void close(void);
    virtual bool seekFrame(qint64);
    virtual bool seekNextFrame(int);
    virtual bool seekMs(int);
    virtual bool getFrame(QImage& img, int* effectiveframenumber, int* effectiveframetime, int* desiredframenumber, int* desiredframetime) const;
    virtual int attributes(void) { return SEEK_NEXT_FRAME; }

signals:

public slots:

private: // variables
    CvCapture* mCamera;

};

#endif // __WEBCAM_H_
