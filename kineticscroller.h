/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#ifndef __KINETICSCROLLER_H_
#define __KINETICSCROLLER_H_

#include <QObject>
#include <QEvent>
#include <QVector>
#include <QElapsedTimer>
#include <QScrollArea>
#include <QPoint>
#include <QTimerEvent>

class KineticScroller : public QObject
{
    Q_OBJECT
public:
    KineticScroller(QScrollArea* scrollArea = NULL, QObject* parent = NULL);
    ~KineticScroller();
    void attachTo(QScrollArea* scrollArea);
    void detach(void);

signals:
    void sizeChanged(QSize);

protected:
    bool eventFilter(QObject* object, QEvent* event);
    void timerEvent(QTimerEvent*);

private:
    static const qreal Friction;
    static const int TimeInterval;
    static const int NumKineticDataSamples;

    struct KineticData {
        KineticData(void) : t(0) { /* ... */ }
        KineticData(const QPoint& p, int t) : p(p), t(t) { /* ... */ }
        QPoint p;
        int t;
    };

    void scrollBy(const QPoint&);
    void startMotion(const QPointF& velocity);
    void stopMotion(void);

    QScrollArea* mScrollArea;
    bool mDragging;
    QPoint mLastMousePos;
    QVector<KineticData> mKineticData;
    QElapsedTimer mMouseMoveTimer;
    int mTimer;
    QPointF mVelocity;
};

#endif // __KINETICSCROLLER_H_
