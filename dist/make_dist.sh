#!/bin/bash

PLUGIN_NAME=QtcMarkview
DIR=$(realpath $(dirname "${BASH_SOURCE[0]}"))
QTC_DIR=$(realpath $DIR/../../qtcreator-latest)
QTC_VERSION=$(cat $QTC_DIR/version)

function makeDist {
    ARCH=$1
    FILE_NAME=$QTC_DIR/$PLUGIN_NAME-$QTC_VERSION-linux-$ARCH.tar.gz
    rm -f $FILE_NAME

    rm -rf dist
    mkdir -p dist/lib/qtcreator/plugins
    mkdir -p dist/share/qtcreator/translations
    if ! $(cp $QTC_DIR/release-$ARCH/lib/qtcreator/plugins/lib$PLUGIN_NAME.so dist/lib/qtcreator/plugins) ; then 
	rm -rf dist
	return;
     fi
    strip dist/lib/qtcreator/plugins/lib$PLUGIN_NAME.so
    cp $DIR/../translation/*.qm dist/share/qtcreator/translations

    cd dist && tar czf $FILE_NAME . && cd ..
    rm -rf dist

    DEP_FILE_NAME=$QTC_DIR/$PLUGIN_NAME-$QTC_VERSION-linux-$ARCH-dep.tar.gz
    rm -f $DEP_FILE_NAME

    mkdir -p dist_dep/bin
    cp $QTC_DIR/Qt/libexec/QtWebEngineProcess dist_dep/bin

    mkdir -p dist_dep/lib/Qt/lib
    cp -P $QTC_DIR/Qt/lib/libQt5WebChannel.so* dist_dep/lib/Qt/lib
    cp -P $QTC_DIR/Qt/lib/libQt5WebEngine.so* dist_dep/lib/Qt/lib
    cp -P $QTC_DIR/Qt/lib/libQt5WebEngineCore.so* dist_dep/lib/Qt/lib
    cp -P $QTC_DIR/Qt/lib/libQt5WebEngineWidgets.so* dist_dep/lib/Qt/lib

    mkdir -p dist_dep/lib/Qt/resources
    cp $QTC_DIR/Qt/resources/* dist_dep/lib/Qt/resources

    cd dist_dep && tar czf $DEP_FILE_NAME . && cd ..
    rm -rf dist_dep
}

makeDist x64
#makeDist x86