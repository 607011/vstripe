/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#ifndef __WEBCAMTHREAD_H_
#define __WEBCAMTHREAD_H_

#include <QThread>
#include <QImage>

class Webcam;

class WebcamThread : public QThread
{
    Q_OBJECT
public:
    WebcamThread(QObject* parent = NULL);
    ~WebcamThread();

    void startReading(void);
    void stopReading(void);
    void setDecoder(Webcam*);

signals:
    void frameReady(QImage);

protected:
    void run(void);

private: // methods
    Webcam* mWebcam;
    bool mAbort;
};

#endif // __WEBCAMTHREAD_H_
