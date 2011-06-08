/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#include "histogram.h"


void Histogram::init(int N)
{
    mN = N;
    mMinBrightness = 2147483647;
    mMaxBrightness = -2147483647-1;
    mTotalBrightness = 0;
    for (HistogramData::iterator i = mBrightness.begin(); i != mBrightness.end(); ++i)
        *i = 0;
}


void Histogram::postprocess(void)
{
    mMaxBrightness = 0;
    for (HistogramData::const_iterator i = mBrightness.constBegin(); i != mBrightness.constEnd(); ++i) {
        if (*i < mMinBrightness)
            mMinBrightness = *i;
        if (*i > mMaxBrightness)
            mMaxBrightness = *i;
    }
    mTotalBrightness /= mN;
}


Histogram& Histogram::operator= (const Histogram &other)
{
    mMinBrightness = other.minBrightness();
    mMaxBrightness = other.maxBrightness();
    mTotalBrightness = other.totalBrightness();
    mBrightness = other.data();
    return *this;
}
