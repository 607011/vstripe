/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#ifndef __HELPBROWSER_H_
#define __HELPBROWSER_H_

#include <QTextBrowser>

QT_BEGIN_NAMESPACE
class QHelpEngineCore;
QT_END_NAMESPACE;

class HelpBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    explicit HelpBrowser(QWidget* parent = NULL);
    void showHelpForKeyword(const QString& id);

private:
    QVariant loadResource(int type, const QUrl &name);
    QHelpEngineCore* mHelpEngine;
};

#endif // __HELPBROWSER_H_
