@echo off
echo Getting the latest version of obfus.h protection from the official repository...
echo.
curl -L https://github.com/DosX-dev/obfus.h/raw/main/include/obfus.h -o obfus.h
if "%errorlevel%" == "0" ( echo Ok! ) else ( pause>nul )