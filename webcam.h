#ifndef __WEBCAM_H_
#define __WEBCAM_H_


#include <QtCore>

#include "abstractvideodecoder.h"


class Webcam : public IAbstractVideoDecoder
{
    Q_OBJECT
public:
    Webcam(QObject* parent = NULL);
    virtual ~Webcam();

signals:

public slots:

private: // variables
    int mCamera;

};

#endif // __WEBCAM_H_
