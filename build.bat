@echo off
mkdir bin
set SOURCES=src/*.c
gcc %SOURCES% -o bin/DesktopShader.exe -I include/ -lopengl32 -lgdi32