#!/bin/bash
# Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
# All rights reserved.

# ----------------------------------------------------
# CHANGE NAMES AND PATHS ACCORDING TO YOUR ENVIRONMENT
# ----------------------------------------------------
APP=VStripe
VERSION=0.9.8.5-x64
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

$QTDIR/bin/lrelease -compress VStripe.pro
cp $APP*.qm $APPBUNDLE/Contents/MacOS

# echo Generating help ..
# $QTDIR/bin/qcollectiongenerator doc.qhcp -o vstripe.qhc

# cp qt.conf $APPBUNDLE/Contents/Resources

echo Bundling libs ..

dylibbundler -od -b -x $APPBUNDLE/Contents/MacOS/$APP -d $APPBUNDLE/Contents/libs

echo Creating directories for Qt frameworks ..

mkdir -pv $APPBUNDLE/Contents/Frameworks/QtCore.framework/Versions/4
mkdir -pv $APPBUNDLE/Contents/Frameworks/QtGui.framework/Versions/4
mkdir -pv $APPBUNDLE/Contents/Frameworks/QtXml.framework/Versions/4
mkdir -pv $APPBUNDLE/Contents/Frameworks/QtSvg.framework/Versions/4
mkdir -pv $APPBUNDLE/Contents/plugins/imageformats

echo Copying Qt framework resources to application bundle ..

cp -Rv $QTLIBDIR/QtCore.framework/Versions/4/Resources $APPBUNDLE/Contents/Frameworks/QtCore.framework
cp -Rv $QTLIBDIR/QtGui.framework/Versions/4/Resources $APPBUNDLE/Contents/Frameworks/QtGui.framework
cp -Rv $QTLIBDIR/QtXml.framework/Versions/4/Resources $APPBUNDLE/Contents/Frameworks/QtXml.framework
cp -Rv $QTLIBDIR/QtSvg.framework/Versions/4/Resources $APPBUNDLE/Contents/Frameworks/QtSvg.framework

echo Copying Qt frameworks to application bundle ..

cp -v $QTLIBDIR/QtCore.framework/Versions/4/QtCore $APPBUNDLE/Contents/Frameworks/QtCore.framework/Versions/4
cp -v $QTLIBDIR/QtGui.framework/Versions/4/QtGui $APPBUNDLE/Contents/Frameworks/QtGui.framework/Versions/4
cp -v $QTLIBDIR/QtXml.framework/Versions/4/QtXml $APPBUNDLE/Contents/Frameworks/QtXml.framework/Versions/4
cp -v $QTLIBDIR/QtSvg.framework/Versions/4/QtSvg $APPBUNDLE/Contents/Frameworks/QtSvg.framework/Versions/4
cp -v $QTLIBDIR/../plugins/imageformats/libqgif.dylib $APPBUNDLE/Contents/plugins/imageformats
cp -v $QTLIBDIR/../plugins/imageformats/libqico.dylib $APPBUNDLE/Contents/plugins/imageformats
cp -v $QTLIBDIR/../plugins/imageformats/libqjpeg.dylib $APPBUNDLE/Contents/plugins/imageformats
cp -v $QTLIBDIR/../plugins/imageformats/libqmng.dylib $APPBUNDLE/Contents/plugins/imageformats
cp -v $QTLIBDIR/../plugins/imageformats/libqsvg.dylib $APPBUNDLE/Contents/plugins/imageformats
cp -v $QTLIBDIR/../plugins/imageformats/libqtiff.dylib $APPBUNDLE/Contents/plugins/imageformats

echo Fixing dependencies on $APP ..

install_name_tool -change $QTLIBDIR/QtCore.framework/Versions/Current/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore $APPBUNDLE/Contents/MacOS/$APP

install_name_tool -change $QTLIBDIR/QtGui.framework/Versions/Current/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui $APPBUNDLE/Contents/MacOS/$APP

install_name_tool -change $QTLIBDIR/QtXml.framework/Versions/Current/QtXml @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml $APPBUNDLE/Contents/MacOS/$APP

# install_name_tool -change $QTLIBDIR/QtSql.framework/Versions/Current/QtSql @executable_path/../Frameworks/QtSql.framework/Versions/4/QtSql $APPBUNDLE/Contents/MacOS/$APP

# install_name_tool -change $QTLIBDIR/QtHelp.framework/Versions/Current/QtHelp @executable_path/../Frameworks/QtHelp.framework/Versions/4/QtHelp $APPBUNDLE/Contents/MacOS/$APP

# install_name_tool -change $QTLIBDIR/QtNetwork.framework/Versions/Current/QtNetwork @executable_path/../Frameworks/QtNetwork.framework/Versions/4/QtNetwork $APPBUNDLE/Contents/MacOS/$APP

echo Fixing dependencies on QtCore ..

install_name_tool -id @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore $APPBUNDLE/Contents/Frameworks/QtCore.framework/Versions/4/QtCore

echo Fixing dependencies on QtXml ..

install_name_tool -id @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml $APPBUNDLE/Contents/Frameworks/QtXml.framework/Versions/4/QtXml

install_name_tool -change $QTLIBDIR/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore $APPBUNDLE/Contents/Frameworks/QtXml.framework/Versions/4/QtXml

echo Fixing dependencies on QtGui ..

install_name_tool -id @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui $APPBUNDLE/Contents/Frameworks/QtGui.framework/Versions/4/QtGui

install_name_tool -change $QTLIBDIR/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore $APPBUNDLE/Contents/Frameworks/QtGui.framework/Versions/4/QtGui

echo Fixing dependencies on QtSvg ..

install_name_tool -id @executable_path/../Frameworks/QtSvg.framework/Versions/4/QtSvg $APPBUNDLE/Contents/Frameworks/QtSvg.framework/Versions/4/QtSvg

install_name_tool -change $QTLIBDIR/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore $APPBUNDLE/Contents/Frameworks/QtSvg.framework/Versions/4/QtSvg

install_name_tool -change $QTLIBDIR/QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui $APPBUNDLE/Contents/Frameworks/QtSvg.framework/Versions/4/QtSvg

echo Fixing dependencies on plugin libqgif.dylib ...

install_name_tool -id @executable_path/../plugins/imageformats/libqgif.dylib $APPBUNDLE/Contents/plugins/imageformats/libqgif.dylib

install_name_tool -change $QTLIBDIR/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore $APPBUNDLE/Contents/plugins/imageformats/libqgif.dylib

install_name_tool -change $QTLIBDIR/QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui $APPBUNDLE/Contents/plugins/imageformats/libqgif.dylib

echo Fixing dependencies on plugin libqico.dylib ...

install_name_tool -id @executable_path/../plugins/imageformats/libqico.dylib $APPBUNDLE/Contents/plugins/imageformats/libqico.dylib

install_name_tool -change $QTLIBDIR/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore $APPBUNDLE/Contents/plugins/imageformats/libqico.dylib

install_name_tool -change $QTLIBDIR/QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui $APPBUNDLE/Contents/plugins/imageformats/libqico.dylib

echo Fixing dependencies on plugin libqmng.dylib ...

install_name_tool -id @executable_path/../plugins/imageformats/libqmng.dylib $APPBUNDLE/Contents/plugins/imageformats/libqmng.dylib

install_name_tool -change $QTLIBDIR/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore $APPBUNDLE/Contents/plugins/imageformats/libqmng.dylib

install_name_tool -change $QTLIBDIR/QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui $APPBUNDLE/Contents/plugins/imageformats/libqmng.dylib

echo Fixing dependencies on plugin libqjpeg.dylib ...

install_name_tool -id @executable_path/../plugins/imageformats/libqjpeg.dylib $APPBUNDLE/Contents/plugins/imageformats/libqjpeg.dylib

install_name_tool -change $QTLIBDIR/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore $APPBUNDLE/Contents/plugins/imageformats/libqjpeg.dylib

install_name_tool -change $QTLIBDIR/QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui $APPBUNDLE/Contents/plugins/imageformats/libqjpeg.dylib

echo Fixing dependencies on plugin libqsvg.dylib ...

install_name_tool -id @executable_path/../plugins/imageformats/libqsvg.dylib $APPBUNDLE/Contents/plugins/imageformats/libqsvg.dylib

install_name_tool -change $QTLIBDIR/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore $APPBUNDLE/Contents/plugins/imageformats/libqsvg.dylib

install_name_tool -change $QTLIBDIR/QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui $APPBUNDLE/Contents/plugins/imageformats/libqsvg.dylib

install_name_tool -change $QTLIBDIR/QtSvg.framework/Versions/4/QtSvg @executable_path/../Frameworks/QtSvg.framework/Versions/4/QtSvg $APPBUNDLE/Contents/plugins/imageformats/libqsvg.dylib

install_name_tool -change $QTLIBDIR/QtXml.framework/Versions/4/QtXml @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml $APPBUNDLE/Contents/plugins/imageformats/libqsvg.dylib

echo Fixing dependencies on plugin libqtiff.dylib ...

install_name_tool -id @executable_path/../plugins/imageformats/libqtiff.dylib $APPBUNDLE/Contents/plugins/imageformats/libqtiff.dylib

install_name_tool -change $QTLIBDIR/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore $APPBUNDLE/Contents/plugins/imageformats/libqtiff.dylib

install_name_tool -change $QTLIBDIR/QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui $APPBUNDLE/Contents/plugins/imageformats/libqtiff.dylib

echo Generating disk image ..

hdiutil create -srcfolder "$APPBUNDLE" -volname "$APP $VERSION" -fs HFS+ -format UDZO -size 100M "$APP-$VERSION.dmg"
