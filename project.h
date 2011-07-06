/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#ifndef __PROJECT_H_
#define __PROJECT_H_

#include <QObject>
#include <QPair>
#include <QString>
#include <QFile>
#include <QRect>
#include <QXmlStreamReader>

class Project : public QObject
{
    Q_OBJECT
public:
    enum MarkId { ID_NONE = 0, ID_A = 1, ID_B = 2 };
    enum FrameType { INVALID_FRAME = -1 };
    static const int MOVING_STRIPE = -1;

    struct mark_type {
        mark_type(int frame = INVALID_FRAME) : id(ID_NONE), frame(frame) {}
        mark_type(MarkId id, int frame) : id(id), frame(frame) {}
        mark_type(MarkId id, int frame, const QString& name) : id(id), frame(frame), name(name) {}
        mark_type(int frame, const QString& name) : id(ID_NONE), frame(frame), name(name) {}
        MarkId id;
        int frame;
        QString name;
        bool isNull(void) const { return frame < 0; }
        bool operator==(const mark_type& other)
        {
            return frame == other.frame &&
                    id == other.id &&
                    name == other.name;
        }
    };

    Project(QObject* parent = NULL);

    Project::mark_type readMarkTag(void);
    void readRegionTag(void);
    void readHistogramTag(void);
    void readMarksTag(void);
    void readInputTag(void);
    void read(void);
    void load(const QString&);
    void load(void);
    void write(void);
    void save(const QString&);
    void save(void);
    void close(void);
    void clearMarks(void);
    int markA(void) const;
    int markB(void) const;
    bool markAIsSet(void) const;
    bool markBIsSet(void) const;
    inline int currentFrame(void) const { return mCurrentFrame; }
    inline const QVector<mark_type>& marks(void) const { return mMarks; }
    inline int stripePos(void) const { return mStripePos; }
    inline bool stripeIsFixed(void) const { return mStripePos >= 0; }
    inline bool stripeIsVertical(void) const { return mVerticalStripe; }
    inline const QString& videoFileName(void) const { return mVideoFileName; }
    inline const QString& fileName(void) const { return mFileName; }
    inline const QRect& histogramRegion(void) const { return mHistogramRegion; }
    inline qreal brightnessLevel(void) const { return mBrightnessLevel; }
    inline qreal redLevel(void) const { return mRedLevel; }
    inline qreal greenLevel(void) const { return mGreenLevel; }
    inline qreal blueLevel(void) const { return mBlueLevel; }
    inline bool modified(void) const { return mModified; }

public slots:
    void setFileName(const QString&);
    void setVideoFileName(const QString&);
    void appendMark(const mark_type&);
    void setCurrentFrame(int);
    void setStripePos(int);
    void setStripeOrientation(bool vertical);
    void setHistogramRegion(const QRect&);
    void setBrightnessLevel(qreal);
    void setRedLevel(qreal);
    void setGreenLevel(qreal);
    void setBlueLevel(qreal);
    void setMarkA(int);
    void setMarkB(int);


private: // members
    QString mFileName;
    QString mVideoFileName;
    QFile mFile;
    QXmlStreamReader mXml;

    QVector<mark_type> mMarks;
    bool mVerticalStripe;
    int mStripePos;
    int mCurrentFrame;
    QRect mHistogramRegion;
    qreal mBrightnessLevel;
    qreal mRedLevel;
    qreal mGreenLevel;
    qreal mBlueLevel;
    bool mModified;

    mark_type* findMark(int id) const;
    const mark_type* findMarkConst(int id) const;

};

#endif // __PROJECT_H_
