#include "webcam.h"

Webcam::Webcam(QObject* parent) : IAbstractVideoDecoder(parent)
{
    mCamera = 0;
}


Webcam::~Webcam()
{

}
