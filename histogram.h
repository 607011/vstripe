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
    Histogram() : mData(WIDTH, 0), mMinBrightness(2147483647), mMaxBrightness(-2147483647-1), mTotalBrightness(0.0), mN(1)
    {
        /* ... */
    }
    Histogram(const Histogram& other) : mData(other.mData), mMinBrightness(other.mMinBrightness), mMaxBrightness(other.mMaxBrightness), mTotalBrightness(other.mTotalBrightness)
    {
        /* ... */
    }
    void init(int);
    inline void add(int val)
    {
        Q_ASSERT(val < WIDTH);
        ++mData[val];
        mTotalBrightness += val;
    }
    void postprocess(void);
    Histogram& operator= (const Histogram&);

public: // variables
    const HistogramData& data(void) const { return mData; }
    inline int minBrightness(void) const { return mMinBrightness; }
    inline int maxBrightness(void) const { return mMaxBrightness; }
    inline qreal totalBrightness(void) const { return mTotalBrightness; }

private:
    HistogramData mData;
    int mMinBrightness;
    int mMaxBrightness;
    qreal mTotalBrightness;
    int mN;
};


Q_DECLARE_METATYPE(Histogram)


#endif // __HISTOGRAM_H_
