/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#include <QtGui/QApplication>
#include <QTranslator>
#include <QtCore/QtDebug>
#include <QtCore/QLocale>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
#if defined(Q_OS_MAC)
    QCoreApplication::addLibraryPath("../plugins");
    qDebug() << QCoreApplication::libraryPaths();
#endif

    QApplication a(argc, argv);

    QTranslator translator;
    translator.load("VStripe_" + QLocale::system().name());
    a.installTranslator(&translator);

    MainWindow w(argc, argv);
    w.show();
    return a.exec();
}
