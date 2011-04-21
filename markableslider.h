/* Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * All rights reserved.
 * $Id$
 */

#ifndef MARKABLESLIDER_H
#define MARKABLESLIDER_H

#include <QSlider>
#include <QVector>

class MarkableSlider : public QSlider
{
    Q_OBJECT
public:
    explicit MarkableSlider(QSlider* parent = NULL);
    QSize minimumSizeHint(void) const { return QSize(200, 16); }
    QSize sizeHint(void) const { return QSize(384, 16); }

    void setA(int);
    void setB(int);
    void setMarks(const QVector<int>&);

protected:
    void paintEvent(QPaintEvent*);

signals:

public slots:

private:
    int mA;
    int mB;
    QVector<int> mMarks;

};

#endif // MARKABLESLIDER_H
