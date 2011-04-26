/* Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * All rights reserved.
 * $Id$
 */

#ifndef PICTUREWIDGET_H
#define PICTUREWIDGET_H

#include <QWidget>
#include <QImage>

class PictureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PictureWidget(QWidget* parent = NULL);
    virtual QSize minimumSizeHint(void) const { return QSize(720, 576); }
    void setVisible(bool visible);
    void setPicture(const QImage&);
    inline const QImage& picture(void) const { return mImage; }

signals:
    void visibilityChanged(bool);

protected:
    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);
    void closeEvent(QCloseEvent*);

public slots:

private:
    QImage mImage;

    static const QString WinTitle;
};

#endif // PICTUREWIDGET_H
