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


void Project::readMarks(void)
{
    //Q_ASSERT(mXml.isStartElement() && mXml.name() == "marks");

    qDebug() << "readMarks()";
    while (mXml.readNextStartElement()) {
        qDebug() << mXml.name();
        if (mXml.name() == "mark") {
            if (mXml.attributes().value("id") == "a")
                mA = mXml.readElementText().toInt();
            else if (mXml.attributes().value("id") == "b")
                mB = mXml.readElementText().toInt();
            else
                mMarks.append(qMakePair(mXml.readElementText().toInt(), QString()));
        }
        else
            mXml.skipCurrentElement();
    }
}


void Project::readInput(void)
{
    //Q_ASSERT(mXml.isStartElement() && mXml.name() == "input");

    qDebug() << "readInput()";
    while (mXml.readNextStartElement()) {
        if (mXml.name() == "file")
            mVideoFileName = mXml.readElementText();
        else
            mXml.skipCurrentElement();
    }
}


bool Project::read(void)
{
    Q_ASSERT(mXml.isStartElement() && mXml.name() == "vstripe");

    mDirty = false;
    while (mXml.readNextStartElement()) {
        qDebug() << mXml.name();
        if (mXml.name() == "input")
            readInput();
        else if (mXml.name() == "marks")
            readMarks();
        else
            mXml.skipCurrentElement();
    }
    return true;
}


bool Project::load(void)
{
    mFile.close();
    mFile.setFileName(mFileName);
    bool rc = mFile.open(QIODevice::ReadOnly);
    if (!rc)
        return false;
    mXml.setDevice(&mFile);
    if (mXml.readNextStartElement()) {
        qDebug() << mXml.name();
        if (mXml.name() != "vstripe") {
            mXml.raiseError(QObject::tr("The file is not an VStripe version 0.1 project file."));
            return false;
        }
        return read();
    }
    return !mXml.error();
}


bool Project::load(const QString& fileName)
{
    mFileName = fileName;
    return load();
}


void Project::save(void)
{
    mFile.close();
    mFileName = "C:/Workspace/VStripe/demo.xml";
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
    mXmlOut.writeTextElement("file", mFileName);
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

