#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QFrame>
#include <QImage>
#include <QPaintEvent>
#include <QResizeEvent>

extern "C" {
#include <avcodec.h>
#include <avformat.h>
#include <swscale.h>
}

class VideoWidget : public QFrame
{
    Q_OBJECT
public:
    explicit VideoWidget(QFrame* parent = 0);
    ~VideoWidget();
    void setFrame(AVFrame* pFrame);

    QSize minimumSizeHint(void) const { return QSize(384, 216); }

protected:
    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);

private:
    QImage image;

signals:

public slots:
    void setFrameSize(int w, int h);
};

#endif // VIDEOWIDGET_H
