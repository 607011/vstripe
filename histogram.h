/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#ifndef __HISTOGRAM_H_
#define __HISTOGRAM_H_

#include <QVector>
#include <QMetaType>
#include <QDebug>

typedef QVector<unsigned int> HistogramData;
typedef QVector<qreal> BrightnessData;


class Histogram {
public:
    static const unsigned int WIDTH = 256;
    Histogram() : mData(WIDTH, 0), mMaxBrightness(0), mTotalBrightness(0), mN(1)
    {
        /* ... */
    }
    Histogram(const Histogram& other) : mData(other.mData), mMaxBrightness(other.mMaxBrightness), mTotalBrightness(other.mTotalBrightness)
    {
        /* ... */
    }
    void init(unsigned int);
    inline void add(unsigned int val)
    {
        Q_ASSERT(val < WIDTH);
        ++mData[val];
        mTotalBrightness += val;
    }
    void postprocess(void);
    Histogram& operator= (const Histogram&);

public: // variables
    const HistogramData& data(void) const { return mData; }
    inline unsigned int maxBrightness(void) const { return mMaxBrightness; }
    inline qreal totalBrightness(void) const { return mTotalBrightness; }

private:
    HistogramData mData;
    unsigned int mMaxBrightness;
    qreal mTotalBrightness;
    unsigned int mN;
};


Q_DECLARE_METATYPE(Histogram)


#endif // __HISTOGRAM_H_
