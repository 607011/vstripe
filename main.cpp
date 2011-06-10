/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#include <QtGui/QApplication>
#include <QTranslator>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    translator.load("VStripe_de");
    a.installTranslator(&translator);

    MainWindow w(argc, argv);
    w.show();
    return a.exec();
}
