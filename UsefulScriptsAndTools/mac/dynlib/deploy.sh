#!/bin/bash

#Variables
BUILD_PATH="./../../EasyAppsClientProject-build-desktop"
QT_GCC_PATH="QtSDK/Desktop/Qt/4.8.1/gcc"
BUILD_DIR="EasyAppsClient"
QUAZIP_PATH="3rdParty/quazip"
APPLICATION_BUNDLE="AppWhirr.app"
EXECUTABLE_NAME="AppWhirr"

#move to the build path
cd $BUILD_PATH

#create Frameworks dir
mkdir -p $BUILD_DIR/$APPLICATION_BUNDLE/Contents/Frameworks
#copy quazip.dynlib
cp $QUAZIP_PATH/libquazip.1.dylib $BUILD_DIR/$APPLICATION_BUNDLE/Contents/Frameworks

#move to AppWhirr path
cd $BUILD_DIR
#copy it into the bundle
install_name_tool -id @executable_path/../Frameworks/libquazip.1.dylib $APPLICATION_BUNDLE/Contents/Frameworks/libquazip.1.dylib
#and for the executable
install_name_tool -change libquazip.1.dylib @executable_path/../Frameworks/libquazip.1.dylib $APPLICATION_BUNDLE/Contents/MacOS/$EXECUTABLE_NAME
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore $APPLICATION_BUNDLE/Contents/Frameworks/libquazip.1.dylib

#deploy
$HOME/$QT_GCC_PATH/bin/macdeployqt $APPLICATION_BUNDLE
