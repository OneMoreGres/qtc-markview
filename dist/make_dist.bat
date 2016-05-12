@echo off

set /p VERSION=<..\..\qtcreator-latest\version
set PLUGIN_NAME=QtcMarkview
set BUILD_DIR=..\..\qtcreator-latest\release-vc12
del %PLUGIN_NAME%-%VERSION%-win.zip

rd /Q /S dist
mkdir dist\lib\qtcreator\plugins
mkdir dist\share\qtcreator\translations
copy /Y %BUILD_DIR%\lib\qtcreator\plugins\%PLUGIN_NAME%.dll        dist\lib\qtcreator\plugins
copy /Y ..\translation\*.qm                                        dist\share\qtcreator\translations

cd dist
zip -q -r ../%PLUGIN_NAME%-%VERSION%-win.zip *
cd ..

del %PLUGIN_NAME%-%VERSION%-win-dep.zip

mkdir dist_dep\bin
copy /Y %BUILD_DIR%\..\Qt\libexec\QtWebEngineProcess dist_dep\bin

mkdir -p dist_dep\lib\Qt\lib
cp -P %BUILD_DIR%\..\Qt\lib\libQt5WebChannel.so* dist_dep\lib\Qt\lib
cp -P %BUILD_DIR%\..\Qt\lib\libQt5WebEngine.so* dist_dep\lib\Qt\lib
cp -P %BUILD_DIR%\..\Qt\lib\libQt5WebEngineCore.so* dist_dep\lib\Qt\lib
cp -P %BUILD_DIR%\..\Qt\lib\libQt5WebEngineWidgets.so* dist_dep\lib\Qt\lib

mkdir -p dist_dep\lib\Qt\resources
cp %BUILD_DIR%\..\Qt\resources\* dist_dep\lib\Qt\resources

cd dist_dep && tar czf $DEP_FILE_NAME . && cd ..
rm -rf dist_dep


rd \Q \S dist
