/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#include "histogram.h"


void Histogram::init(unsigned int N)
{
    mN = N;
    mMaxBrightness = 0;
    mTotalBrightness = 0;
    for (HistogramData::iterator i = mData.begin(); i != mData.end(); ++i)
        *i = 0;
}


void Histogram::postprocess(void)
{
    mMaxBrightness = 0;
    for (HistogramData::const_iterator i = mData.constBegin(); i != mData.constEnd(); ++i)
        if (*i > mMaxBrightness)
            mMaxBrightness = *i;
    mTotalBrightness /= mN;
    qDebug() << "mMaxBrightness =" << mMaxBrightness << ", mTotalBrightness =" << mTotalBrightness << ", mN =" << mN;
}


Histogram& Histogram::operator= (const Histogram &other)
{
    mMaxBrightness = other.maxBrightness();
    mTotalBrightness = other.totalBrightness();
    mData = other.data();
    return *this;
}
