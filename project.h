/* Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * All rights reserved.
 * $Id$
 */

#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>
#include <QPair>
#include <QString>
#include <QFile>
#include <QXmlStreamReader>

class Project : public QObject
{
    Q_OBJECT
public:
    enum { ID_NONE = 0, ID_A, ID_B };
    enum { INVALID_FRAME = -1 };

    struct mark_type {
        mark_type(int frame = Project::INVALID_FRAME) : id(Project::ID_NONE), frame(frame) {}
        mark_type(int id, int frame) : id(id), frame(frame) {}
        mark_type(int id, int frame, const QString& name) : id(id), frame(frame), name(name) {}
        mark_type(int frame, const QString& name) : id(Project::ID_NONE), frame(frame), name(name) {}
        int id;
        int frame;
        QString name;
        bool isNull(void) const { return frame < 0; }
    };

    explicit Project(QObject* parent = NULL);

    Project::mark_type readMarkTag(void);
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
    inline int stripeWidth(void) const { return mStripeWidth; }
    inline int stripePos(void) const { return mStripePos; }
    inline bool stripeIsFixed(void) const { return mFixedStripe; }
    inline bool stripeIsVertical(void) const { return mVerticalStripe; }
    inline const QString& videoFileName(void) const { return mVideoFileName; }
    inline const QString& fileName(void) const { return mFileName; }
    inline bool modified(void) const { return mModified; }

public slots:
    void setFileName(const QString&);
    void setVideoFileName(const QString&);
    void appendMark(const mark_type&);
    void setCurrentFrame(int);
    void setStripePos(int);
    void setStripeOrientation(bool vertical);
    void setFixed(bool);
    void setMarkA(int);
    void setMarkB(int);


private: // members
    QString mFileName;
    QString mVideoFileName;
    QFile mFile;
    QXmlStreamReader mXml;

    QVector<mark_type> mMarks;
    int mStripeWidth; // Streifen dieser Breite (Pixel) werden von jedem eingelesenen Frame behalten
    bool mFixedStripe; // true: der Streifen bleibt fest an der gewählten Position; false: der Streifen bewegt sich mit jedem Frame um mStripeWidth Pixel weiter
    bool mVerticalStripe;
    int mStripePos;
    int mCurrentFrame;
    bool mModified;

    mark_type* findMark(int id) const;
    const mark_type* findMarkConst(int id) const;

};

#endif // PROJECT_H
