#!/bin/bash
# Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
# All rights reserved.

# ----------------------------------------------------
# CHANGE NAMES AND PATHS ACCORDING TO YOUR ENVIRONMENT
# ----------------------------------------------------
APP=VStripe
VERSION=0.9.2
QTDIR=/Users/olau/QtSDK/Desktop/Qt/473/gcc
QTLIBDIR=$QTDIR/lib

# ----------------------------------------
# DO NOT CHANGE ANYTHING BENEATH THIS LINE
# ----------------------------------------
APPBUNDLE=./$APP.app

rm -Rf $APPBUNDLE
rm $APP-$VERSION.dmg

qmake
make clean
make CONFIG=release

echo Bundling libs ..
dylibbundler -od -b -x $APPBUNDLE/Contents/MacOS/$APP -d $APPBUNDLE/Contents/libs

echo Creating directories for Qt frameworks ..
mkdir $APPBUNDLE/Contents
mkdir $APPBUNDLE/Contents/Frameworks
mkdir $APPBUNDLE/Contents/Frameworks/QtCore.framework
mkdir $APPBUNDLE/Contents/Frameworks/QtGui.framework
mkdir $APPBUNDLE/Contents/Frameworks/QtXml.framework
mkdir $APPBUNDLE/Contents/Frameworks/QtCore.framework/Versions
mkdir $APPBUNDLE/Contents/Frameworks/QtGui.framework/Versions
mkdir $APPBUNDLE/Contents/Frameworks/QtXml.framework/Versions
mkdir $APPBUNDLE/Contents/Frameworks/QtCore.framework/Versions/4
mkdir $APPBUNDLE/Contents/Frameworks/QtGui.framework/Versions/4
mkdir $APPBUNDLE/Contents/Frameworks/QtXml.framework/Versions/4

echo Copying Qt framework resources to application bundle ..
cp -Rv $QTLIBDIR/QtCore.framework/Resources $APPBUNDLE/Contents/Frameworks/QtCore.framework
cp -Rv $QTLIBDIR/QtGui.framework/Resources $APPBUNDLE/Contents/Frameworks/QtGui.framework
cp -Rv $QTLIBDIR/QtXml.framework/Resources $APPBUNDLE/Contents/Frameworks/QtXml.framework

echo Copying Qt frameworks to application bundle ..
cp -Rv $QTLIBDIR/QtCore.framework/Versions/4/QtCore $APPBUNDLE/Contents/Frameworks/QtCore.framework/Versions/4
cp -Rv $QTLIBDIR/QtGui.framework/Versions/4/QtGui $APPBUNDLE/Contents/Frameworks/QtGui.framework/Versions/4
cp -Rv $QTLIBDIR/QtXml.framework/Versions/4/QtXml $APPBUNDLE/Contents/Frameworks/QtXml.framework/Versions/4

echo Generating DMG ..
hdiutil create -srcfolder "$APPBUNDLE" -volname "$APP $VERSION" -fs HFS+ -format UDZO -size 100M "$APP-$VERSION.dmg"
