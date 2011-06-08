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
    for (HistogramData::iterator i = mRed.begin(); i != mRed.end(); ++i)
        *i = 0;
    for (HistogramData::iterator i = mGreen.begin(); i != mGreen.end(); ++i)
        *i = 0;
    for (HistogramData::iterator i = mBlue.begin(); i != mBlue.end(); ++i)
        *i = 0;
}


void Histogram::add(QRgb rgb)
{
    const int l = QColor(rgb).lightness();
    ++mBrightness[l];
    mTotalBrightness += l;
    if (l < mMinBrightness)
        mMinBrightness = l;
    if (l > mMaxBrightness)
        mMaxBrightness = l;

    const int r = qRed(rgb);
    ++mRed[r];
    mTotalRed += r;
    if (r < mMinRed)
        mMinRed = r;
    if (r > mMaxRed)
        mMaxRed = r;

    const int g = qGreen(rgb);
    ++mGreen[g];
    mTotalGreen += g;
    if (g < mMinGreen)
        mMinGreen = g;
    if (g > mMaxGreen)
        mMaxGreen = g;

    const int b = qBlue(rgb);
    ++mBlue[b];
    mTotalBlue += b;
    if (b < mMinBlue)
        mMinBlue = b;
    if (b > mMaxBlue)
        mMaxBlue = b;
}


void Histogram::postprocess(void)
{
    mTotalBrightness /= mN;
    mTotalRed /= mN;
    mTotalGreen /= mN;
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
