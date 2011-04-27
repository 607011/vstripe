/* Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * All rights reserved.
 * $Id$
 */

#ifndef PICTUREWIDGET_H
#define PICTUREWIDGET_H

#include <QWidget>
#include <QImage>
#include <QResizeEvent>

class PictureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PictureWidget(QWidget* parent = NULL);
    virtual QSize minimumSizeHint(void) const { return QSize(720, 576); }
    void setVisible(bool visible);
    void setPicture(const QImage&);
    void setSizeConstraint(const QSize&, const QSize&);
    inline const QImage& picture(void) const { return mImage; }

signals:
    void visibilityChanged(bool);
    void sizeChanged(const QSize&);

protected:
    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);
    void closeEvent(QCloseEvent*);

private:
    QImage mImage;

    static const QString WinTitle;
};

#endif // PICTUREWIDGET_H
