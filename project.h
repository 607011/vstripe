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
    enum { ID_NONE = 0, ID_A = 1, ID_B = 2 };
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

    inline bool isDirty(void) const { return mDirty; }
    int markA(void) const;
    int markB(void) const;
    bool markAIsSet(void) const;
    bool markBIsSet(void) const;
    inline const QVector<mark_type>& marks(void) const { return mMarks; }
    inline int stripeWidth(void) const { return mStripeWidth; }
    inline bool stripeIsFixed(void) const { return mFixedStripe; }
    inline bool stripeIsVertical(void) const { return mVerticalStripe; }

    void setFixed(bool);
    void setMarkA(int);
    void setMarkB(int);

    void appendMark(const mark_type&);
    void clearMarks(void);

    const QString& videoFileName(void) const { return mVideoFileName; }
    void setVideoFileName(const QString&);

    const QString& fileName(void) const { return mFileName; }
    void setFileName(const QString&);

private: // members
    QString mFileName;
    QString mVideoFileName;
    QFile mFile;
    QXmlStreamReader mXml;

    QVector<mark_type> mMarks;
    int mStripeWidth; // Streifen dieser Breite (Pixel) werden von jedem eingelesenen Frame behalten
    bool mFixedStripe; // true: der Streifen bleibt fest an der gewählten Position; false: der Streifen bewegt sich mit jedem Frame um mStripeWidth Pixel weiter
    bool mVerticalStripe;
    bool mDirty;

    mark_type* findMark(int id) const;
    const mark_type* findMarkConst(int id) const;


public slots:

signals:

};

#endif // PROJECT_H
