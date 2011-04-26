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
#include <QXmlStreamWriter>
#include <QXmlStreamReader>


class Project : public QObject
{
    Q_OBJECT
public:
    typedef QPair<int, QString> mark_type;

    explicit Project(QObject* parent = NULL);

    int readMarkTag(void);
    void readMarksTag(void);
    void readInputTag(void);
    void read(void);

    void load(const QString&);
    void load(void);

    void write(void);

    void save(const QString&);
    void save(void);

    void close(void);

    inline int markA(void) const { return mA; }
    inline int markB(void) const { return mB; }
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

private: // members
    QString mFileName;
    QString mVideoFileName;
    QFile mFile;
    QXmlStreamReader mXml;
    QXmlStreamWriter mXmlOut;

    QVector<mark_type> mMarks;
    int mA;
    int mB;
    int mStripeWidth; // Streifen dieser Breite (Pixel) werden von jedem eingelesenen Frame behalten
    bool mFixedStripe; // true: der Streifen bleibt fest an der gewählten Position; false: der Streifen bewegt sich mit jedem Frame um mStripeWidth Pixel weiter
    bool mVerticalStripe;
    bool mDirty;


public slots:

signals:

};

#endif // PROJECT_H
