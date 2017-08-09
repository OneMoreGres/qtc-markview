#!/bin/bash

set -e

SELF_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

export PATH="$PWD/qtcreator-latest/Qt/bin/:$PATH"
ROOT="$SELF_PATH/../.."
echo `clang --version`
echo `qmake --version`


export QTC_SOURCE="$PWD/qtcreator-latest/src"
export QTC_BUILD="$PWD/qtcreator-latest/compiled"

rm -rf build
mkdir -p build

cd build
lrelease "$ROOT/qtc-markview.pro"
qmake "$ROOT/qtc-markview.pro"
make
cd ..


PLUGIN_NAME="QtcMarkview"
VERSION=$(cat qtcreator-latest/version)

if [ ! -f "qtcreator-latest/compiled/Contents/PlugIns/lib$PLUGIN_NAME.dylib" ]; then exit 1; fi

rm -rf dist
mkdir -p dist/Contents/PlugIns/
mkdir -p dist/Contents/Resources/translations
cp qtcreator-latest/compiled/Contents/PlugIns/lib$PLUGIN_NAME.dylib dist/Contents/PlugIns/
ls -l dist/Contents/PlugIns/lib$PLUGIN_NAME.dylib
cp $ROOT/translation/*.qm dist/Contents/Resources/translations

rm -f $PLUGIN_NAME-$VERSION-osx.tar.gz
cd dist
tar czf ../$PLUGIN_NAME-$VERSION-osx.tar.gz .
cd ..


rm -rf dist_dep
mkdir -p dist_dep/Contents/Frameworks
cp -R qtcreator-latest/Qt/lib/QtWebChannel.framework dist_dep/Contents/Frameworks
cp -R qtcreator-latest/Qt/lib/QtWebEngine.framework dist_dep/Contents/Frameworks
cp -R qtcreator-latest/Qt/lib/QtWebEngineCore.framework dist_dep/Contents/Frameworks
cp -R qtcreator-latest/Qt/lib/QtWebEngineWidgets.framework dist_dep/Contents/Frameworks

find dist_dep/Contents/Frameworks -type f -name '*_debug' -exec rm {} \;

rm -f $PLUGIN_NAME-$VERSION-osx-dep.tar.gz
cd dist_dep
tar czf ../$PLUGIN_NAME-$VERSION-osx-dep.tar.gz . 
cd ..

ls -l $PLUGIN_NAME-$VERSION-osx.tar.gz
ls -l $PLUGIN_NAME-$VERSION-osx-dep.tar.gz
