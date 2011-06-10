/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#ifndef __MARKABLESLIDER_H_
#define __MARKABLESLIDER_H_

#include <QSlider>
#include <QVector>
#include "project.h"

class MarkableSlider : public QSlider
{
    Q_OBJECT
public:
    MarkableSlider(const Project* project, QSlider* parent = NULL);
    QSize minimumSizeHint(void) const { return QSize(200, 16); }
    QSize sizeHint(void) const { return QSize(384, 16); }

protected:
    void paintEvent(QPaintEvent*);

signals:

public slots:

private:
    const Project* mProject;

};

#endif // __MARKABLESLIDER_H_
