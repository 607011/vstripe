/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#include "kineticscroller.h"

#include <QMouseEvent>
#include <QScrollBar>
#include <QtCore/QtDebug>
#include <qmath.h>

const qreal KineticScroller::Friction = 0.85;
const int KineticScroller::TimeInterval = 20;
const int KineticScroller::NumKineticDataSamples = 5;


KineticScroller::KineticScroller(QObject* parent) :
    QObject(parent),
    mScrollArea(NULL),
    mDragging(false),
    mTimer(0)
{
    /* ... */
}


KineticScroller::~KineticScroller()
{
    stopMotion();
    detach();
}


void KineticScroller::startMotion(const QPointF& velocity)
{
    mVelocity = velocity;
    if (mTimer == 0)
        mTimer = startTimer(TimeInterval);
}


void KineticScroller::stopMotion(void)
{
    if (mTimer) {
        killTimer(mTimer);
        mTimer = 0;
    }
}


void KineticScroller::detach(void)
{
    if (mScrollArea)
        mScrollArea->viewport()->removeEventFilter(this);
}


void KineticScroller::attachTo(QScrollArea* scrollArea)
{
    detach();
    mScrollArea = scrollArea;
    mScrollArea->viewport()->installEventFilter(this);
}


bool KineticScroller::eventFilter(QObject* object, QEvent* event)
{
    Q_ASSERT_X(mScrollArea != NULL, "KineticScroller", "QScrollArea not set");
    Q_ASSERT_X(object == mScrollArea->viewport(), "KineticScroller", "Invalid QScrollArea");
    const QMouseEvent* const mouseEvent = reinterpret_cast<const QMouseEvent*>(event);
    bool doFilterEvent = true;
    switch (event->type()) {
    case QEvent::MouseButtonPress:
        if (mouseEvent->button() == Qt::LeftButton) {
            stopMotion();
            mScrollArea->viewport()->setCursor(Qt::ClosedHandCursor);
            mLastMousePos = mouseEvent->pos();
            mDragging = true;
            mMouseMoveTimer.start();
            mKineticData.clear();
        }
        break;
    case QEvent::MouseMove:
        if (mDragging) {
            scrollBy(mLastMousePos - mouseEvent->pos());
            mKineticData.push_back(KineticData(mouseEvent->pos(), mMouseMoveTimer.elapsed()));
            if (mKineticData.count() > NumKineticDataSamples)
                mKineticData.pop_front();
            mLastMousePos = mouseEvent->pos();
        }
        break;
    case QEvent::MouseButtonRelease:
        if (mDragging) {
            mDragging = false;
            mScrollArea->viewport()->setCursor(Qt::OpenHandCursor);
            if (mKineticData.count() == NumKineticDataSamples) {
                const int timeSinceLastMoveEvent = mMouseMoveTimer.elapsed() - mKineticData.last().t;
                if (timeSinceLastMoveEvent < 100) {
                    const QPoint dp = mKineticData.first().p - mouseEvent->pos();
                    const int dt = mMouseMoveTimer.elapsed() - mKineticData.first().t;
                    startMotion(1000 * dp / dt / TimeInterval);
                }
            }
        }
        break;
    case QEvent::Resize:
        {
            const QResizeEvent* const resizeEvent = reinterpret_cast<const QResizeEvent*>(event);
            emit sizeChanged(resizeEvent->size());
        }
        break;
    case QEvent::KeyPress:
        {
            const QKeyEvent* const keyEvent = reinterpret_cast<const QKeyEvent*>(event);
            qDebug() << "keyEvent->key() =" << keyEvent->key();
        }
        break;
    default:
        doFilterEvent = false;
        break;
    }

    return doFilterEvent;
}


void KineticScroller::timerEvent(QTimerEvent*)
{
    if (mVelocity.manhattanLength() > M_SQRT2) {
        scrollBy(mVelocity.toPoint());
        mVelocity *= Friction;
    }
    else {
        stopMotion();
        mVelocity = QPointF(0, 0);
    }
}


void KineticScroller::scrollBy(const QPoint& d)
{
    Q_ASSERT_X(mScrollArea != NULL, "KineticScroller", "QScrollArea not set");
    mScrollArea->horizontalScrollBar()->setValue(mScrollArea->horizontalScrollBar()->value() + d.x());
    mScrollArea->verticalScrollBar()->setValue(mScrollArea->verticalScrollBar()->value() + d.y());
}
