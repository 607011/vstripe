#ifndef __WEBCAM_H_
#define __WEBCAM_H_

#include <QTimer>
#include "abstractvideodecoder.h"


namespace cv {
    class VideoCapture;
}


class Webcam : public IAbstractVideoDecoder
{
    Q_OBJECT
public:
    Webcam(QObject* parent = NULL);
    virtual ~Webcam();

    // IAbstractVideoDecoder methods
    virtual inline bool open(const char*) { return false; }
    virtual bool open(int deviceId);
    virtual void close(void);
    virtual bool seekNextFrame(int);
    virtual bool getFrame(QImage& img, int* effectiveframenumber = 0, int* effectiveframetime = 0, int* desiredframenumber = 0, int* desiredframetime = 0);
    virtual inline bool seekFrame(qint64) { return false; }
    virtual inline bool seekMs(int) { return false; }
    virtual inline QSize frameSize() const { return mFrameSize; }
    virtual inline int getVideoLengthMs(void) { return -1; }
    virtual inline QString codecInfo(void) const { return QString(); }
    virtual inline const QString typeName(void) const { return "Webcam"; }

    // other methods
    void setSize(const QSize&);

private: // variables
    cv::VideoCapture* mCamera;
    QImage mLastFrame;
    QSize mFrameSize;
    int mFrameNumber;
    int mFrameTime;
};

#endif // __WEBCAM_H_
