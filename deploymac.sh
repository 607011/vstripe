#!/bin/bash
# Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
# All rights reserved.

# ----------------------------------------------------
# CHANGE NAMES AND PATHS ACCORDING TO YOUR ENVIRONMENT
# ----------------------------------------------------
APP=VStripe
VERSION=0.9.2
QTDIR=/Developer/QtSDK/Desktop/Qt/473/gcc
QTLIBDIR=$QTDIR/lib

# ----------------------------------------
# DO NOT CHANGE ANYTHING BENEATH THIS LINE
# ----------------------------------------
APPBUNDLE=./$APP.app

rm $APP-$VERSION.dmg

rm -Rf $APPBUNDLE

qmake CONFIG+=release
make clean
make 

echo Making translations ..
$QTDIR/bin/lrelease VStripe.pro
cp $APP*.qm $APPBUNDLE/Contents/MacOS

echo Bundling libs ..

dylibbundler -od -b -x $APPBUNDLE/Contents/MacOS/$APP -d $APPBUNDLE/Contents/libs

echo Creating directories for Qt frameworks ..

mkdir -pv $APPBUNDLE/Contents/Frameworks/QtCore.framework/Versions/4
mkdir -pv $APPBUNDLE/Contents/Frameworks/QtGui.framework/Versions/4
mkdir -pv $APPBUNDLE/Contents/Frameworks/QtXml.framework/Versions/4

echo Copying Qt framework resources to application bundle ..

cp -Rv $QTLIBDIR/QtCore.framework/Versions/4/Resources $APPBUNDLE/Contents/Frameworks/QtCore.framework
cp -Rv $QTLIBDIR/QtGui.framework/Versions/4/Resources $APPBUNDLE/Contents/Frameworks/QtGui.framework
cp -Rv $QTLIBDIR/QtXml.framework/Versions/4/Resources $APPBUNDLE/Contents/Frameworks/QtXml.framework

echo Copying Qt frameworks to application bundle ..

cp -v $QTLIBDIR/QtCore.framework/Versions/4/QtCore $APPBUNDLE/Contents/Frameworks/QtCore.framework/Versions/4
cp -v $QTLIBDIR/QtGui.framework/Versions/4/QtGui $APPBUNDLE/Contents/Frameworks/QtGui.framework/Versions/4
cp -v $QTLIBDIR/QtXml.framework/Versions/4/QtXml $APPBUNDLE/Contents/Frameworks/QtXml.framework/Versions/4

echo Fixing dependencies on $APP ..

install_name_tool -change $QTLIBDIR/QtCore.framework/Versions/Current/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore $APPBUNDLE/Contents/MacOS/$APP

install_name_tool -change $QTLIBDIR/QtGui.framework/Versions/Current/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui $APPBUNDLE/Contents/MacOS/$APP

install_name_tool -change $QTLIBDIR/QtXml.framework/Versions/Current/QtXml @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml $APPBUNDLE/Contents/MacOS/$APP

echo Fixing dependencies on QtCore ..

install_name_tool -id @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore $APPBUNDLE/Contents/Frameworks/QtCore.framework/Versions/4/QtCore

echo Fixing dependencies on QtXml ..

install_name_tool -id @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml $APPBUNDLE/Contents/Frameworks/QtXml.framework/Versions/4/QtXml

install_name_tool -change $QTLIBDIR/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore $APPBUNDLE/Contents/Frameworks/QtXml.framework/Versions/4/QtXml

echo Fixing dependencies on QtGui ..

install_name_tool -id @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui $APPBUNDLE/Contents/Frameworks/QtGui.framework/Versions/4/QtGui

install_name_tool -change $QTLIBDIR/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore $APPBUNDLE/Contents/Frameworks/QtGui.framework/Versions/4/QtGui


echo Generating disk image ..

hdiutil create -srcfolder "$APPBUNDLE" -volname "$APP $VERSION" -fs HFS+ -format UDZO -size 100M "$APP-$VERSION.dmg"
