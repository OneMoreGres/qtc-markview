@echo off

set /p VERSION=<..\..\qtcreator-latest\version
set PLUGIN_NAME=QtcMarkview
set BUILD_DIR=..\..\qtcreator-latest\release
del %PLUGIN_NAME%-%VERSION%-win.zip

rd /Q /S dist
mkdir dist\lib\qtcreator\plugins
mkdir dist\share\qtcreator\translations
copy /Y %BUILD_DIR%\lib\qtcreator\plugins\%PLUGIN_NAME%4.dll        dist\lib\qtcreator\plugins
copy /Y ..\translation\*.qm                                        dist\share\qtcreator\translations

cd dist
zip -q -r ../%PLUGIN_NAME%-%VERSION%-win.zip *
cd ..

rd /Q /S dist

del %PLUGIN_NAME%-%VERSION%-win-dep.zip

rd /Q /S dist_dep
mkdir dist_dep\bin
copy /Y %BUILD_DIR%\..\Qt\bin\QtWebEngineProcess.exe dist_dep\bin

copy /Y %BUILD_DIR%\..\Qt\bin\Qt5WebChannel.dll dist_dep\bin
copy /Y %BUILD_DIR%\..\Qt\bin\Qt5WebEngine.dll dist_dep\bin
copy /Y %BUILD_DIR%\..\Qt\bin\Qt5WebEngineCore.dll dist_dep\bin
copy /Y %BUILD_DIR%\..\Qt\bin\Qt5WebEngineWidgets.dll dist_dep\bin

mkdir dist_dep\bin\resources
copy /Y %BUILD_DIR%\..\Qt\resources dist_dep\bin\resources

cd dist_dep
zip -q -r ../%PLUGIN_NAME%-%VERSION%-win-dep.zip *
cd ..

rd /Q /S dist_dep
