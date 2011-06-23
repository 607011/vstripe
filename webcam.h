#ifndef __WEBCAM_H_
#define __WEBCAM_H_

#include <QTimer>
#include "abstractvideodecoder.h"

struct CvCapture;

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
    virtual bool getFrame(QImage& img, int* effectiveframenumber, int* effectiveframetime, int* desiredframenumber = 0, int* desiredframetime = 0) const;
    virtual int attributes(void) { return SEEK_NEXT_FRAME; }
    virtual QSize frameSize() const;
    virtual int getVideoLengthMs(void);
    virtual QString codecInfo(void) const;

signals:

public slots:

private: // variables
    CvCapture* mCamera;
    QImage mLastFrame;
    QSize mFrameSize;
    int mFrameNumber;
    int mFrameTime;
};

#endif // __WEBCAM_H_
