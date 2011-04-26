/* Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * All rights reserved.
 * $Id$
 */

#include <QDebug>
#include "project.h"

Project::Project(QObject *parent) :
    QObject(parent), mA(-1), mB(-1), mStripeWidth(1), mFixedStripe(false), mVerticalStripe(true), mDirty(false)
{
}


int Project::readMarkTag(void)
{
    Q_ASSERT(mXml.isStartElement() && mXml.name() == "mark");

    qDebug() << "readMarkTag()";
    while (!(mXml.tokenType() == QXmlStreamReader::EndElement && mXml.name() == "mark")) {
        qDebug() << mXml.name();
        if (mXml.tokenType() == QXmlStreamReader::StartElement) {
            if (mXml.name() == "frame")
                return mXml.readElementText().toInt();
            else if (mXml.name() == "name") {
                qDebug() << mXml.name();
                continue;
            }
            else continue;
        }
        mXml.readNext();
    }
    return -1;
}


void Project::readMarksTag(void)
{
    Q_ASSERT(mXml.isStartElement() && mXml.name() == "marks");

    qDebug() << "readMarksTag()";
    while (!(mXml.tokenType() == QXmlStreamReader::EndElement && mXml.name() == "marks")) {
        if (mXml.tokenType() == QXmlStreamReader::StartElement) {
            if (mXml.name() == "mark") {
                qDebug() << "mark found.";
                if (mXml.attributes().value("id") == "a") {
                    const int mark = readMarkTag();
                    if (mark >= 0)
                        mA = mark;
                    qDebug() << "mA = " << mA;
                }
                else if (mXml.attributes().value("id") == "b") {
                    const int mark = readMarkTag();
                    if (mark >= 0)
                        mB = mark;
                    qDebug() << "mB = " << mB;
                }
                else {
                    // TODO
                    const int mark = readMarkTag();
                    if (mark >= 0)
                        mMarks.append(qMakePair(mark, QString()));
                }
            }
        }
        mXml.readNext();
    }
    if (mMarks.size() > 0)
        qSort(mMarks);
}


void Project::readInputTag(void)
{
    Q_ASSERT(mXml.isStartElement() && mXml.name() == "input");

    qDebug() << "readInputTag()";
    while (mXml.readNextStartElement()) {
        if (mXml.name() == "file")
            mVideoFileName = mXml.readElementText();
        else mXml.skipCurrentElement();
    }
}


void Project::read(void)
{
    mDirty = false;
    while (!mXml.atEnd() && !mXml.hasError() && mXml.readNextStartElement()) {
        if (mXml.name() == "input")
            readInputTag();
        else if (mXml.name() == "marks")
            readMarksTag();
        else mXml.skipCurrentElement();
    }
    mXml.clear();
}


void Project::load(void)
{
    mFile.close();
    mFile.setFileName(mFileName);
    bool rc = mFile.open(QIODevice::ReadOnly);
    if (!rc)
        return;
    mXml.setDevice(&mFile);
    if (!mXml.atEnd() && !mXml.hasError() && mXml.readNextStartElement()) {
        if (mXml.name() == "vstripe")
            read();
        else
            mXml.raiseError(QObject::tr("The file is not an VStripe project file."));
    }
}


void Project::load(const QString& fileName)
{
    mFileName = fileName;
    load();
}


void Project::save(void)
{
    mFile.close();
    mFile.setFileName(mFileName);
    bool rc = mFile.open(QIODevice::WriteOnly);
    if (!rc)
        return;
    mXmlOut.setAutoFormatting(true);
    mXmlOut.setAutoFormattingIndent(-1);
    mXmlOut.setDevice(&mFile);
    mXmlOut.writeStartDocument();
    mXmlOut.writeStartElement("vstripe");
    mXmlOut.writeStartElement("input");
    mXmlOut.writeTextElement("file", mVideoFileName);
    mXmlOut.writeEndElement();

    if (mA > 0 || mB > 0 || mMarks.size() > 0) {
        mXmlOut.writeStartElement("marks");
        if (mA > 0) {
            mXmlOut.writeStartElement("mark");
            mXmlOut.writeAttribute("id", "a");
            mXmlOut.writeTextElement("frame", QString("%1").arg(mA));
            mXmlOut.writeEndElement();
        }
        if (mB > 0) {
            mXmlOut.writeStartElement("mark");
            mXmlOut.writeAttribute("id", "b");
            mXmlOut.writeTextElement("frame", QString("%1").arg(mB));
            mXmlOut.writeEndElement();
        }
        foreach (mark_type m, mMarks) {
            mXmlOut.writeStartElement("mark");
            mXmlOut.writeTextElement("frame", QString("%1").arg(m.first));
            mXmlOut.writeTextElement("name", m.second);
            mXmlOut.writeEndElement();

        }
        mXmlOut.writeEndElement();
    }
    mXmlOut.writeEndDocument();
    mFile.close();
}


void Project::save(const QString& fileName)
{
    mFileName = fileName;
    save();
}


void Project::close(void)
{
    if (mFile.isOpen())
        mFile.close();
}


void Project::setVideoFileName(const QString& fileName)
{
    mVideoFileName = fileName;
}


void Project::setFixed(bool fixed)
{
    mFixedStripe = fixed;
}


void Project::setMarkA(int a)
{
    mA = a;
}


void Project::setMarkB(int b)
{
    mB = b;
}


void Project::appendMark(const mark_type& mark)
{
    mMarks.append(mark);
    qSort(mMarks);
}


void Project::clearMarks(void)
{
    mMarks.clear();
}

