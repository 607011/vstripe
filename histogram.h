/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#ifndef __HISTOGRAM_H_
#define __HISTOGRAM_H_

#include <QVector>
#include <QMetaType>
#include <QDebug>

typedef QVector<int> HistogramData;
typedef QVector<qreal> BrightnessData;


class Histogram {
public:
    static const int WIDTH = 256;
    Histogram() : mBrightness(WIDTH, 0), mMinBrightness(2147483647), mMaxBrightness(-2147483647-1), mTotalBrightness(0.0), mN(1)
    {
        /* ... */
    }
    Histogram(const Histogram& other) : mBrightness(other.mBrightness), mMinBrightness(other.mMinBrightness), mMaxBrightness(other.mMaxBrightness), mTotalBrightness(other.mTotalBrightness)
    {
        /* ... */
    }
    void init(int);
    inline void add(int val)
    {
        Q_ASSERT(val < WIDTH);
        ++mBrightness[val];
        mTotalBrightness += val;
    }
    void postprocess(void);
    Histogram& operator= (const Histogram&);

    const HistogramData& data(void) const { return mBrightness; }
    const HistogramData& red(void) const { return mRed; }
    const HistogramData& green(void) const { return mGreen; }
    const HistogramData& blue(void) const { return mBlue; }
    inline int minBrightness(void) const { return mMinBrightness; }
    inline int maxBrightness(void) const { return mMaxBrightness; }
    inline int minRed(void) const { return mMinRed; }
    inline int maxRed(void) const { return mMaxRed; }
    inline int minGreen(void) const { return mMinGreen; }
    inline int maxGreen(void) const { return mMaxGreen; }
    inline int minBlue(void) const { return mMinBlue; }
    inline int maxBlue(void) const { return mMaxBlue; }
    inline qreal totalBrightness(void) const { return mTotalBrightness; }
    inline qreal totalRed(void) const { return mTotalRed; }
    inline qreal totalGreen(void) const { return mTotalGreen; }
    inline qreal totalBlue(void) const { return mTotalBlue; }

private:// variables
    HistogramData mBrightness;
    HistogramData mRed;
    HistogramData mGreen;
    HistogramData mBlue;
    int mMinBrightness, mMaxBrightness;
    int mMinRed, mMaxRed;
    int mMinGreen, mMaxGreen;
    int mMinBlue, mMaxBlue;
    qreal mTotalBrightness;
    qreal mTotalRed;
    qreal mTotalGreen;
    qreal mTotalBlue;
    int mN;
};


Q_DECLARE_METATYPE(Histogram)


#endif // __HISTOGRAM_H_
