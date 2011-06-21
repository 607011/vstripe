/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#include <QtCore/QLibraryInfo>
#include <QtGui/QApplication>
#include <QtHelp/QHelpEngineCore>

#include "helpbrowser.h"


HelpBrowser::HelpBrowser(QWidget* parent) : QTextBrowser(parent)
{
    mHelpEngine = new QHelpEngineCore("doc/vstripe.qhc", this);
    if (!mHelpEngine->setupData()) {
        delete mHelpEngine;
        mHelpEngine = NULL;
    }
}


void HelpBrowser::showHelpForKeyword(const QString &id)
{
    if (mHelpEngine) {
        QMap<QString, QUrl> links = mHelpEngine->linksForIdentifier(id);
        if (links.count())
            setSource(links.constBegin().value());
    }
}


QVariant HelpBrowser::loadResource(int type, const QUrl &name)
{
    QByteArray ba;
    if (type < 4 && mHelpEngine) {
        QUrl url(name);
        if (name.isRelative())
            url = source().resolved(url);
        ba = mHelpEngine->fileData(url);
    }
    return ba;
}
