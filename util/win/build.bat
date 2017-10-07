@echo off

set SELF_PATH=%~dp0
call %SELF_PATH%\env.bat

set QTC_SOURCE=%cd%\qtcreator-latest\src
set QTC_BUILD=%cd%\qtcreator-latest\compiled

rmdir /s /q build
mkdir build

cd build
lrelease %SELF_PATH%\..\..\qtc-markview.pro
qmake %SELF_PATH%\..\..
nmake
if %errorlevel% neq 0 exit /b %errorlevel%
cd ..


set /p VERSION=<qtcreator-latest\version
set PLUGIN_NAME=QtcMarkview

if not exist qtcreator-latest\compiled\lib\qtcreator\plugins\%PLUGIN_NAME%4.dll  exit /b 1

rd /Q /S dist
mkdir dist\lib\qtcreator\plugins
mkdir dist\share\qtcreator\translations
copy /Y qtcreator-latest\compiled\lib\qtcreator\plugins\%PLUGIN_NAME%4.dll dist\lib\qtcreator\plugins
if %errorlevel% neq 0 exit /b %errorlevel%
copy /Y %SELF_PATH%\..\..\translation\*.qm dist\share\qtcreator\translations

if exist %PLUGIN_NAME%-%VERSION%-win.zip del /Q %PLUGIN_NAME%-%VERSION%-win.zip
cd dist
7z a ..\%PLUGIN_NAME%-%VERSION%-win.zip *
cd ..


rd /Q /S dist_dep
mkdir dist_dep\bin
copy /Y qtcreator-latest\Qt\bin\QtWebEngineProcess.exe dist_dep\bin
copy /Y qtcreator-latest\Qt\bin\Qt5WebChannel.dll dist_dep\bin
copy /Y qtcreator-latest\Qt\bin\Qt5WebEngine.dll dist_dep\bin
copy /Y qtcreator-latest\Qt\bin\Qt5WebEngineCore.dll dist_dep\bin
copy /Y qtcreator-latest\Qt\bin\Qt5WebEngineWidgets.dll dist_dep\bin

mkdir dist_dep\bin\resources
copy /Y qtcreator-latest\Qt\resources dist_dep\bin\resources

if exist %PLUGIN_NAME%-%VERSION%-win-dep.zip del /Q %PLUGIN_NAME%-%VERSION%-win-dep.zip
cd dist_dep
7z a ..\%PLUGIN_NAME%-%VERSION%-win-dep.zip *
cd ..

