@echo off 

SETLOCAL

mkdir build
pushd build

set TARGET=x64

:: SDL2 libraries and include paths
set SDL2_LIB=path\to\SDL2\installation

set "SDL2_LIBDIR=%SDL2_LIB%\lib\%TARGET%"
set "SDL2_LIBINC=%SDL2_LIB%\include"

rem robocopy %SDL2_LIBDIR% . *.dll

cl ..\ppmviewer.cpp /I %SDL2_LIBINC% /link %SDL2_LIBDIR%\SDL2.lib %SDL2_LIBDIR%\SDL2main.lib /SUBSYSTEM:CONSOLE

popd
