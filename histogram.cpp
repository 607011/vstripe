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
    mMinRed = 2147483647;
    mMaxRed = -2147483647-1;
    mMinGreen = 2147483647;
    mMaxGreen = -2147483647-1;
    mMinBlue = 2147483647;
    mMaxBlue = -2147483647-1;
    mTotalBrightness = 0;
    mTotalRed = 0;
    mTotalGreen = 0;
    mTotalBlue = 0;
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

    mMaxRed = 0;
    for (HistogramData::const_iterator i = mRed.constBegin(); i != mRed.constEnd(); ++i) {
        if (*i < mMinRed)
            mMinRed = *i;
        if (*i > mMaxRed)
            mMaxRed = *i;
    }
    mTotalRed /= mN;

    mMaxGreen = 0;
    for (HistogramData::const_iterator i = mGreen.constBegin(); i != mGreen.constEnd(); ++i) {
        if (*i < mMinGreen)
            mMinGreen = *i;
        if (*i > mMaxGreen)
            mMaxGreen = *i;
    }
    mTotalGreen /= mN;

    mMaxBlue = 0;
    for (HistogramData::const_iterator i = mBlue.constBegin(); i != mBlue.constEnd(); ++i) {
        if (*i < mMinBlue)
            mMinBlue = *i;
        if (*i > mMaxBlue)
            mMaxBlue = *i;
    }
    mTotalBlue /= mN;
}


Histogram& Histogram::operator= (const Histogram &other)
{
    mMinBrightness = other.minBrightness();
    mMaxBrightness = other.maxBrightness();
    mTotalBrightness = other.totalBrightness();
    mBrightness = other.brightness();

    mMinRed = other.minRed();
    mMaxRed = other.maxRed();
    mTotalRed = other.totalRed();
    mRed = other.red();

    mMinGreen = other.minGreen();
    mMaxGreen = other.maxGreen();
    mTotalGreen = other.totalGreen();
    mGreen = other.green();

    mMinBlue = other.minBlue();
    mMaxBlue = other.maxBlue();
    mTotalBlue = other.totalBlue();
    mBlue = other.blue();

    return *this;
}
