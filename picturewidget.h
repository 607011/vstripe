/* Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * All rights reserved.
 * $Id$
 */

#ifndef PICTUREWIDGET_H
#define PICTUREWIDGET_H

#include <QWidget>

class PictureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PictureWidget(QWidget* parent = 0);

    QSize minimumSizeHint(void) const { return QSize(720, 576); }
    void setVisible(bool visible);

signals:
    void visibilityChanged(bool);

protected:
    void closeEvent(QCloseEvent*);

public slots:

};

#endif // PICTUREWIDGET_H
