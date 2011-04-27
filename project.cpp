/* Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
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
    QObject(parent), mStripeWidth(1), mFixedStripe(false), mVerticalStripe(true), mDirty(false)
{
}


Project::mark_type Project::readMarkTag(void)
{
    Q_ASSERT(mXml.isStartElement() && mXml.name() == "mark");

    int frame = Project::INVALID_FRAME;
    QString name;
    int id;
    while (!(mXml.tokenType() == QXmlStreamReader::EndElement && mXml.name() == "mark")) {
        if (mXml.tokenType() == QXmlStreamReader::StartElement) {
            if (mXml.name() == "mark") {
                QString _id = mXml.attributes().value("id").toString();
                qDebug() << "<" << mXml.name() << " id=" << _id << ">";
                if (_id == "a")
                    id = Project::ID_A;
                else if (_id == "b")
                    id = Project::ID_B;
                else if (_id == "recent")
                    id = Project::ID_RECENT;
                else
                    id = Project::ID_NONE;
            }
            else if (mXml.name() == "frame")
                frame = mXml.readElementText().toInt(), qDebug() << "frame = " << frame;
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
                if (mark.frame != Project::INVALID_FRAME) {
                    if (mark.id == Project::ID_RECENT)
                        mCurrentFrame = mark.frame;
                    else
                        mMarks.append(mark);
                }
            }
        }
        mXml.readNext();
    }
    if (mMarks.size() > 0)
        qSort(mMarks.begin(), mMarks.end(), markLess);
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
    clearMarks();
    if (!mXml.atEnd() && !mXml.hasError() && mXml.readNextStartElement()) {
        if (mXml.name() == "vstripe")
            read();
        else
            mXml.raiseError(QObject::tr("The file is not a VStripe project file."));
    }
    mDirty = false;
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
        xml.writeStartElement("mark");
        xml.writeAttribute("id", "recent");
        xml.writeTextElement("frame", QString("%1").arg(mCurrentFrame));
        xml.writeEndElement();
        xml.writeEndElement();
    }
    xml.writeEndDocument();
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
    mDirty = false;
}


void Project::setVideoFileName(const QString& fileName)
{
    mVideoFileName = fileName;
    mDirty = true;
}


void Project::setFileName(const QString& fileName)
{
    mFileName = fileName;
    mDirty = true;
}


void Project::setFixed(bool fixed)
{
    mFixedStripe = fixed;
    mDirty = true;
}


void Project::setCurrentFrame(int frame)
{
    mCurrentFrame = frame;
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
        if (a->frame == Project::INVALID_FRAME)
            mMarks.erase(a);
        else
            a->frame = frame;
    }
    mDirty = true;
}


void Project::setMarkB(int frame)
{
    mark_type* b = findMark(Project::ID_B);
    if (b == NULL)
        mMarks.append(Project::mark_type(Project::ID_B, frame));
    else {
        if (b->frame == Project::INVALID_FRAME)
            mMarks.erase(b);
        else
            b->frame = frame;
    }
    mDirty = true;
}


void Project::appendMark(const Project::mark_type& mark)
{
    mMarks.append(mark);
    qSort(mMarks.begin(), mMarks.end(), markLess);
    mDirty = true;
}


void Project::clearMarks(void)
{
    mMarks.clear();
    mDirty = true;
}
