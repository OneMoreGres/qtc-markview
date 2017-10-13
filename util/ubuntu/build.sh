#!/bin/bash

set -e

SELF_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

GCC_PATH=/opt/gcc-5/
export PATH="$GCC_PATH:$PWD/qtcreator-latest/Qt/bin/:$PATH"
ROOT="$SELF_PATH/../.."
echo `g++ --version`
echo `qmake --version`

export QTC_SOURCE="$PWD/qtcreator-latest/src"
export QTC_BUILD="$PWD/qtcreator-latest/compiled"

rm -rf build
mkdir -p build

cd build
lrelease "$ROOT/qtc-markview.pro"
qmake "$ROOT/qtc-markview.pro"
make -j`nproc`
cd ..


PLUGIN_NAME="QtcMarkview"
VERSION=$(cat qtcreator-latest/version)

if [ ! -f "qtcreator-latest/compiled/lib/qtcreator/plugins/lib$PLUGIN_NAME.so" ]; then exit 1; fi

rm -rf dist
mkdir -p dist/lib/qtcreator/plugins
mkdir -p dist/share/qtcreator/translations
cp qtcreator-latest/compiled/lib/qtcreator/plugins/lib$PLUGIN_NAME.so dist/lib/qtcreator/plugins
strip dist/lib/qtcreator/plugins/lib$PLUGIN_NAME.so
ls -l dist/lib/qtcreator/plugins/lib$PLUGIN_NAME.so
cp $SELF_PATH/../../translation/*.qm dist/share/qtcreator/translations

rm -f $PLUGIN_NAME-$VERSION-linux-x64.tar.gz
cd dist
tar czf ../$PLUGIN_NAME-$VERSION-linux-x64.tar.gz . 
cd ..


mkdir -p dist_dep/bin
cp qtcreator-latest/Qt/libexec/QtWebEngineProcess dist_dep/bin
mkdir -p dist_dep/lib/Qt/lib
cp -P qtcreator-latest/Qt/lib/libQt5WebChannel.so* dist_dep/lib/Qt/lib
cp -P qtcreator-latest/Qt/lib/libQt5WebEngine.so* dist_dep/lib/Qt/lib
cp -P qtcreator-latest/Qt/lib/libQt5WebEngineCore.so* dist_dep/lib/Qt/lib
cp -P qtcreator-latest/Qt/lib/libQt5WebEngineWidgets.so* dist_dep/lib/Qt/lib
mkdir -p dist_dep/lib/Qt/resources
cp qtcreator-latest/Qt/resources/* dist_dep/lib/Qt/resources

rm -f $PLUGIN_NAME-$VERSION-linux-x64-dep.tar.gz
cd dist_dep
tar czf ../$PLUGIN_NAME-$VERSION-linux-x64-dep.tar.gz .
cd ..

ls -l $PLUGIN_NAME-$VERSION-linux-x64.tar.gz
ls -l $PLUGIN_NAME-$VERSION-linux-x64-dep.tar.gz
