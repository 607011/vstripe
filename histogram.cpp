/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#include "histogram.h"


void Histogram::init(int N)
{
    mN = 1.0 / N;
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
    for (HistogramData::iterator i = mRed.begin(); i != mRed.end(); ++i)
        *i = 0;
    for (HistogramData::iterator i = mGreen.begin(); i != mGreen.end(); ++i)
        *i = 0;
    for (HistogramData::iterator i = mBlue.begin(); i != mBlue.end(); ++i)
        *i = 0;
}


void Histogram::add(QRgb rgb)
{
    const int l = qGray(rgb); // QColor(rgb).lightness();
    ++mBrightness[l];
    mTotalBrightness += l;
    const int r = qRed(rgb);
    ++mRed[r];
    mTotalRed += r;
    const int g = qGreen(rgb);
    ++mGreen[g];
    mTotalGreen += g;
    const int b = qBlue(rgb);
    ++mBlue[b];
    mTotalBlue += b;
}


void Histogram::postprocess(void)
{
    mTotalBrightness *= mN;
    mTotalRed *= mN;
    mTotalGreen *= mN;
    mTotalBlue *= mN;

    for (HistogramData::iterator i = mBrightness.begin(); i != mBrightness.end(); ++i) {
        if (*i < mMinBrightness)
            mMinBrightness = *i;
        if (*i > mMaxBrightness)
            mMaxBrightness = *i;
    }

    for (HistogramData::iterator i = mRed.begin(); i != mRed.end(); ++i) {
        if (*i < mMinRed)
            mMinRed = *i;
        if (*i > mMaxRed)
            mMaxRed = *i;
    }

    for (HistogramData::iterator i = mGreen.begin(); i != mGreen.end(); ++i) {
        if (*i < mMinGreen)
            mMinGreen = *i;
        if (*i > mMaxGreen)
            mMaxGreen = *i;
    }

    for (HistogramData::iterator i = mBlue.begin(); i != mBlue.end(); ++i) {
        if (*i < mMinBlue)
            mMinBlue = *i;
        if (*i > mMaxBlue)
            mMaxBlue = *i;
    }
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

    mN = other.mN;

    return *this;
}
