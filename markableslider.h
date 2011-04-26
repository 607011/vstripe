/* Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * All rights reserved.
 * $Id$
 */

#ifndef MARKABLESLIDER_H
#define MARKABLESLIDER_H

#include <QSlider>
#include <QVector>
#include "project.h"

class MarkableSlider : public QSlider
{
    Q_OBJECT
public:
    explicit MarkableSlider(const Project* project, QSlider* parent = NULL);
    virtual QSize minimumSizeHint(void) const { return QSize(200, 16); }
    virtual QSize sizeHint(void) const { return QSize(384, 16); }

protected:
    void paintEvent(QPaintEvent*);

signals:

public slots:

private:
    const Project* mProject;

};

#endif // MARKABLESLIDER_H
