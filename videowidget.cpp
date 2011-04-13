#include <QPainter>
#include <QDebug>

#include "videowidget.h"

VideoWidget::VideoWidget(QFrame* parent) : QFrame(parent)
{
    setFrameStyle(QFrame::Box | QFrame::Sunken);
    setBaseSize(480, 270);
    setMinimumSize(384, 216);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setSizeIncrement(16, 9);
}


VideoWidget::~VideoWidget() {
}


void VideoWidget::setFrameSize(int w, int h) {
    image = QImage(w, h, QImage::Format_RGB32);
    qDebug() << "image size = " << QSize(w, h);
}


void VideoWidget::setFrame(AVFrame* pFrame)
{
    for (int y = 0; y < image.height(); ++y) {
        quint8* scanLine = reinterpret_cast<quint8*>(image.scanLine(y));
        for (int x = 0; x < image.width(); ++x) {
            scanLine++;
            *scanLine++ = *(pFrame->data[0] + y*pFrame->linesize[0]+x+0);
            *scanLine++ = *(pFrame->data[0] + y*pFrame->linesize[0]+x+1);
            *scanLine++ = *(pFrame->data[0] + y*pFrame->linesize[0]+x+2);
        }
    }
    update();
}


void VideoWidget::resizeEvent(QResizeEvent* e)
{
    qDebug() << "resized to " << e->size();
}


void VideoWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.drawImage(0, 0, image, 0, 0, width(), height());
}
