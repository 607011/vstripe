/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#include "kineticscroller.h"

#include <QMouseEvent>
#include <QScrollBar>
#include <QtCore/QtDebug>
#include <qmath.h>

const qreal KineticScroller::Friction = 0.6;
const int KineticScroller::TimeInterval = 30;
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
    detach();
    if (mTimer)
        killTimer(mTimer);
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
            mScrollArea->viewport()->setCursor(Qt::ClosedHandCursor);
            mDragStartPos = mouseEvent->pos();
            mOrigScrollBarValue = QPoint(mScrollArea->horizontalScrollBar()->value(), mScrollArea->verticalScrollBar()->value());
            mDragging = true;
            mMouseMoveTimer.start();
            mKineticData.clear();
        }
        break;
    case QEvent::MouseMove:
        if (mDragging) {
            scrollBy(mDragStartPos - mouseEvent->pos());
            mKineticData.push_back(KineticData(mouseEvent->pos(), mMouseMoveTimer.elapsed()));
            if (mKineticData.count() > NumKineticDataSamples)
                mKineticData.pop_front();
        }
        break;
    case QEvent::MouseButtonRelease:
        if (mDragging) {
            mDragging = false;
            mScrollArea->viewport()->setCursor(Qt::OpenHandCursor);
            if (mKineticData.count() == NumKineticDataSamples) {
                mOrigScrollBarValue = QPoint(mScrollArea->horizontalScrollBar()->value(), mScrollArea->verticalScrollBar()->value());
                QPoint dp(mKineticData.first().p - mouseEvent->pos());
                qDebug() << "KineticScroller::eventFilter() dp =" << dp;
                int dt = mMouseMoveTimer.elapsed() - mKineticData.first().t;
                mVelocity = 1000 * dp / dt / TimeInterval;
                qDebug() << "KineticScroller::eventFilter() mVelocity =" << mVelocity;
                if (mTimer == 0)
                    mTimer = startTimer(TimeInterval);
            }
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
        scrollBy(-mVelocity.toPoint());
        mVelocity *= Friction;
    }
    else {
        killTimer(mTimer);
        mTimer = 0;
        mVelocity = QPointF(0, 0);
    }
    qDebug() << "KineticScroller::timerEvent() mVelocity =" << mVelocity;
}


void KineticScroller::scrollBy(const QPoint& d)
{
    Q_ASSERT_X(mScrollArea != NULL, "KineticScroller", "QScrollArea not set");
    mScrollArea->horizontalScrollBar()->setValue(mOrigScrollBarValue.x() + d.x());
    mScrollArea->verticalScrollBar()->setValue(mOrigScrollBarValue.y() + d.y());
}
