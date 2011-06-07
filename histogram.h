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

class Histogram {
public:
    Histogram() : mData(256, 0), mMaxBrightness(0.0), mTotalBrightness(0.0), mN(1)
    {
        /* ... */
    }
    Histogram(const Histogram& other) : mData(other.mData), mMaxBrightness(other.mMaxBrightness), mTotalBrightness(other.mTotalBrightness)
    {
        /* ... */
    }
    void init(unsigned int);
    inline void add(unsigned int v0)
    {
        ++mData[v0];
        mTotalBrightness += v0;
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
