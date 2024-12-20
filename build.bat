@echo off
IF NOT EXIST bin mkdir bin
SET SOURCES=src/*.c
gcc %SOURCES% -o bin/DesktopShader.exe -I include/ -lopengl32 -lgdi32