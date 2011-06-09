/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#ifndef __HISTOGRAM_H_
#define __HISTOGRAM_H_

#include <QVector>
#include <QRgb>
#include <QColor>
#include <QMetaType>
#include <QDebug>

typedef QVector<int> HistogramData;
typedef QVector<qreal> BrightnessData;


class Histogram {
public:
    static const int WIDTH = 256;
    Histogram() :
            mBrightness(WIDTH, 0),
            mRed(WIDTH, 0),
            mGreen(WIDTH, 0),
            mBlue(WIDTH, 0),
            mMinBrightness(2147483647),
            mMaxBrightness(-2147483647-1),
            mMinRed(2147483647),
            mMaxRed(-2147483647-1),
            mMinGreen(2147483647),
            mMaxGreen(-2147483647-1),
            mMinBlue(2147483647),
            mMaxBlue(-2147483647-1),
            mTotalBrightness(0.0),
            mTotalRed(0.0),
            mTotalGreen(0.0),
            mTotalBlue(0.0),
            mN(0)
    {
        /* ... */
    }
    Histogram(const Histogram& other) :
            mBrightness(other.mBrightness),
            mRed(other.mRed),
            mGreen(other.mGreen),
            mBlue(other.mBlue),
            mMinBrightness(other.mMinBrightness),
            mMaxBrightness(other.mMaxBrightness),
            mMinRed(other.mMinRed),
            mMaxRed(other.mMaxRed),
            mMinGreen(other.mMinGreen),
            mMaxGreen(other.mMaxGreen),
            mMinBlue(other.mMinBlue),
            mMaxBlue(other.mMaxBlue),
            mTotalBrightness(other.mTotalBrightness),
            mTotalRed(other.mTotalRed),
            mTotalGreen(other.mTotalGreen),
            mTotalBlue(other.mTotalBlue),
            mN(other.mN)
    {
        /* ... */
    }
    void init(int);
    void add(QRgb rgb);
    void postprocess(void);
    Histogram& operator= (const Histogram&);
    inline const HistogramData& brightness(void) const { return mBrightness; }
    inline const HistogramData& red(void) const { return mRed; }
    inline const HistogramData& green(void) const { return mGreen; }
    inline const HistogramData& blue(void) const { return mBlue; }
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

private: // variables
    HistogramData mBrightness;
    HistogramData mRed;
    HistogramData mGreen;
    HistogramData mBlue;
    int mMinBrightness;
    int mMaxBrightness;
    int mMinRed;
    int mMaxRed;
    int mMinGreen;
    int mMaxGreen;
    int mMinBlue;
    int mMaxBlue;
    qreal mTotalBrightness;
    qreal mTotalRed;
    qreal mTotalGreen;
    qreal mTotalBlue;
    qreal mN;
};


Q_DECLARE_METATYPE(Histogram)


#endif // __HISTOGRAM_H_
