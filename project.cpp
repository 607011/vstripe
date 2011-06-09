/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#include <QDebug>
#include <QXmlStreamWriter>
#include "project.h"


bool markLess(const Project::mark_type& m1, const Project::mark_type& m2)
{
    return m1.frame < m2.frame;
}


Project::Project(QObject* parent) :
    QObject(parent),
    mFixedStripe(false),
    mVerticalStripe(true),
    mBrightnessLevel(0.7),
    mRedLevel(0),
    mGreenLevel(0),
    mBlueLevel(0),
    mModified(false)
{
}


Project::mark_type Project::readMarkTag(void)
{
    Q_ASSERT(mXml.isStartElement() && mXml.name() == "mark");

    int frame = Project::INVALID_FRAME;
    QString name;
    MarkId id = Project::ID_NONE;
    while (!(mXml.tokenType() == QXmlStreamReader::EndElement && mXml.name() == "mark")) {
        if (mXml.tokenType() == QXmlStreamReader::StartElement) {
            if (mXml.name() == "mark") {
                QString _id = mXml.attributes().value("id").toString();
                if (_id == "a")
                    id = Project::ID_A;
                else if (_id == "b")
                    id = Project::ID_B;
                else
                    id = Project::ID_NONE;
            }
            else if (mXml.name() == "frame")
                frame = mXml.readElementText().toInt();
            else if (mXml.name() == "name")
                name = mXml.readElementText();
        }
        mXml.readNext();
    }
    return Project::mark_type(id, frame, name);
}


void Project::readMarksTag(void)
{
    Q_ASSERT(mXml.isStartElement() && mXml.name() == "marks");

    while (!(mXml.tokenType() == QXmlStreamReader::EndElement && mXml.name() == "marks")) {
        if (mXml.tokenType() == QXmlStreamReader::StartElement) {
            if (mXml.name() == "mark") {
                mark_type mark = readMarkTag();
                if (mark.frame != Project::INVALID_FRAME)
                    mMarks.append(mark);
            }
        }
        mXml.readNext();
    }
    qSort(mMarks.begin(), mMarks.end(), markLess);
}


void Project::readRegionTag(void)
{
    Q_ASSERT(mXml.isStartElement() && mXml.name() == "region");

    while (!(mXml.tokenType() == QXmlStreamReader::EndElement && mXml.name() == "region")) {
        if (mXml.tokenType() == QXmlStreamReader::StartElement) {
            if (mXml.name() == "x")
                mHistogramRegion.setLeft(mXml.readElementText().toInt());
            else if (mXml.name() == "y")
                mHistogramRegion.setTop(mXml.readElementText().toInt());
            else if (mXml.name() == "width")
                mHistogramRegion.setWidth(mXml.readElementText().toInt());
            else if (mXml.name() == "height")
                mHistogramRegion.setHeight(mXml.readElementText().toInt());
        }
        mXml.readNext();
    }
}


void Project::readHistogramTag(void)
{
    Q_ASSERT(mXml.isStartElement() && mXml.name() == "histogram");

    while (!(mXml.tokenType() == QXmlStreamReader::EndElement && mXml.name() == "histogram")) {
        if (mXml.tokenType() == QXmlStreamReader::StartElement) {
            if (mXml.name() == "region") {
                readRegionTag();
            }
            else if (mXml.name() == "level") {
                QString channel = mXml.attributes().value("channel").toString();
                if (channel == "brightness")
                    mBrightnessLevel = mXml.readElementText().toDouble();
                else if (channel == "red")
                    mRedLevel = mXml.readElementText().toDouble();
                else if (channel == "green")
                    mGreenLevel = mXml.readElementText().toDouble();
                else if (channel == "blue")
                    mBlueLevel = mXml.readElementText().toDouble();
            }
        }
        mXml.readNext();
    }
}


void Project::readInputTag(void)
{
    Q_ASSERT(mXml.isStartElement() && mXml.name() == "input");

    while (mXml.readNextStartElement()) {
        if (mXml.name() == "file")
            mVideoFileName = mXml.readElementText();
        else mXml.skipCurrentElement();
    }
}


void Project::read(void)
{
    Q_ASSERT(mXml.isStartElement() && mXml.name() == "vstripe");

    while (!mXml.atEnd() && !mXml.hasError() && mXml.readNextStartElement()) {
        if (mXml.name() == "input")
            readInputTag();
        else if (mXml.name() == "marks")
            readMarksTag();
        else if (mXml.name() == "histogram")
            readHistogramTag();
        else if (mXml.name() == "recent")
            mCurrentFrame = mXml.readElementText().toInt();
        else if (mXml.name() == "stripe") {
            mVerticalStripe = mXml.attributes().value("orientation") == "vertical";
            mStripePos = mXml.readElementText().toInt();
        }
        else mXml.skipCurrentElement();
    }
    mXml.clear();
}


void Project::load(void)
{
    if (mFile.isOpen())
        mFile.close();
    mFile.setFileName(mFileName);
    bool rc = mFile.open(QIODevice::ReadOnly);
    if (!rc)
        return;
    mXml.setDevice(&mFile);
    clearMarks();
    if (!mXml.atEnd() && !mXml.hasError() && mXml.readNextStartElement()) {
        if (mXml.name() == "vstripe")
            read();
        else
            mXml.raiseError(QObject::tr("The file is not a VStripe project file."));
    }
    mModified = false;
}


void Project::load(const QString& fileName)
{
    mFileName = fileName;
    load();
}


void Project::save(void)
{
    if (mFile.isOpen())
        mFile.close();
    mFile.setFileName(mFileName);
    bool rc = mFile.open(QIODevice::WriteOnly);
    if (!rc)
        return;
    QXmlStreamWriter xml(&mFile);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(-1);
    xml.writeStartDocument();
    xml.writeStartElement("vstripe");
    xml.writeStartElement("input");
    xml.writeTextElement("file", mVideoFileName);
    xml.writeEndElement();

    if (mMarks.size() > 0) {
        xml.writeStartElement("marks");
        foreach (mark_type m, mMarks) {
            if (m.frame == Project::INVALID_FRAME)
                continue;
            xml.writeStartElement("mark");
            if (m.id == Project::ID_A)
                xml.writeAttribute("id", "a");
            else if (m.id == Project::ID_B)
                xml.writeAttribute("id", "b");
            xml.writeTextElement("frame", QString("%1").arg(m.frame));
            if (!m.name.isNull())
                xml.writeTextElement("name", m.name);
            xml.writeEndElement();
        }
        xml.writeEndElement(); // marks
    }
    xml.writeTextElement("recent", QString("%1").arg(mCurrentFrame));
    xml.writeStartElement("histogram");
    if (!mHistogramRegion.isEmpty()) {
        xml.writeStartElement("region");
        xml.writeTextElement("x", QString("%1").arg(mHistogramRegion.x()));
        xml.writeTextElement("y", QString("%1").arg(mHistogramRegion.y()));
        xml.writeTextElement("width", QString("%1").arg(mHistogramRegion.width()));
        xml.writeTextElement("height", QString("%1").arg(mHistogramRegion.height()));
        xml.writeEndElement(); // region
    }
    xml.writeStartElement("level");
    xml.writeAttribute("channel", "brightness");
    xml.writeCharacters(QString("%1").arg(mBrightnessLevel));
    xml.writeEndElement();
    xml.writeStartElement("level");
    xml.writeAttribute("channel", "red");
    xml.writeCharacters(QString("%1").arg(mRedLevel));
    xml.writeEndElement();
    xml.writeStartElement("level");
    xml.writeAttribute("channel", "green");
    xml.writeCharacters(QString("%1").arg(mGreenLevel));
    xml.writeEndElement();
    xml.writeStartElement("level");
    xml.writeAttribute("channel", "blue");
    xml.writeCharacters(QString("%1").arg(mBlueLevel));
    xml.writeEndElement();
    xml.writeEndElement(); // histogram

    xml.writeStartElement("stripe");
    xml.writeAttribute("orientation", mVerticalStripe? "vertical" : "horizontal");
    xml.writeCharacters(QString("%1").arg(mStripePos));
    xml.writeEndElement(); // stripe

    xml.writeEndDocument();
    mFile.close();
    mModified = false;
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
    mModified = false;
}


void Project::setVideoFileName(const QString& fileName)
{
    mVideoFileName = fileName;
    mModified = true;
}


void Project::setFileName(const QString& fileName)
{
    mFileName = fileName;
    mModified = true;
}


void Project::setFixed(bool fixed)
{
    mFixedStripe = fixed;
    mModified = true;
}


void Project::setCurrentFrame(int frame)
{
    mCurrentFrame = frame;
    mModified = true;
}


void Project::setStripePos(int pos)
{
    mStripePos = pos;
    mModified = true;
}


void Project::setStripeOrientation(bool vertical)
{
    mVerticalStripe = vertical;
    mModified = true;
}


void Project::setHistogramRegion(const QRect& region)
{
    mHistogramRegion = region;
}


void Project::setBrightnessLevel(qreal level)
{
    mBrightnessLevel = level;
}


void Project::setRedLevel(qreal level)
{
    mRedLevel = level;
}


void Project::setGreenLevel(qreal level)
{
    mGreenLevel = level;
}


void Project::setBlueLevel(qreal level)
{
    mBlueLevel = level;
}


Project::mark_type* Project::findMark(int id) const
{
    for (QVector<mark_type>::const_iterator m = mMarks.constBegin(); m != mMarks.constEnd(); ++m)
        if (m->id == id)
            return const_cast<Project::mark_type*>(m);
    return NULL;
}


const Project::mark_type* Project::findMarkConst(int id) const
{
    return (const Project::mark_type*)findMark(id);
}


int Project::markA(void) const
{
    const mark_type* a = findMarkConst(Project::ID_A);
    return (a != NULL)? a->frame : Project::INVALID_FRAME;
}


int Project::markB(void) const
{
    const mark_type* b = findMarkConst(Project::ID_B);
    return (b != NULL)? b->frame : Project::INVALID_FRAME;
}


bool Project::markAIsSet(void) const
{
    return findMarkConst(Project::ID_A) != NULL;
}


bool Project::markBIsSet(void) const
{
    return findMarkConst(Project::ID_B) != NULL;
}


void Project::setMarkA(int frame)
{
    mark_type* a = findMark(Project::ID_A);
    if (a == NULL)
        mMarks.append(Project::mark_type(Project::ID_A, frame));
    else {
        if (frame == Project::INVALID_FRAME)
            mMarks.erase(a);
        else
            a->frame = frame;
    }
    mModified = true;
}


void Project::setMarkB(int frame)
{
    mark_type* b = findMark(Project::ID_B);
    if (b == NULL)
        mMarks.append(Project::mark_type(Project::ID_B, frame));
    else {
        if (frame == Project::INVALID_FRAME)
            mMarks.erase(b);
        else
            b->frame = frame;
    }
    mModified = true;
}


void Project::appendMark(const Project::mark_type& mark)
{
    mMarks.append(mark);
    qSort(mMarks.begin(), mMarks.end(), markLess);
    mModified = true;
}


void Project::clearMarks(void)
{
    mMarks.clear();
    mModified = true;
}
