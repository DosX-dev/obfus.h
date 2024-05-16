@echo off
echo.
set "protocol=https"
set "host=github.com:443"
set "obfus_h_path=DosX-dev/raw/main/include/obfus.h"

:: Current directory
set "output_dir=./"

set "special=--ssl-no-revoke -s"

:: If error:
::  0 - pause
::  1 - continue (not recommended)
set "onerror="

call:log "Getting the latest version of obfus.h protection from %host%..."
curl -L %special% "%protocol%://%host%/%obfus_h_path%" -o "%output_dir%\obfus.h"
if "%errorlevel%" == "0" (
    call:log "Latest version of protection has been installed."
) else (
    call:log "An error occurred while downloading the component package."
    if "%onerror%" == "0" pause>nul
)

exit /b 0

:log
echo [%time%][pkg:obfus.h] %~1
goto :eof
