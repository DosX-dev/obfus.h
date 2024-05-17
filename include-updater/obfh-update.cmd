@echo off
call:print "         _       __                 _     "
call:print "   ___  | |__   / _| _   _  ___    | |__  "
call:print "  / _ \ | '_ \ | |_ | | | |/ __|   | '_ \ "
call:print " | (_) || |_) ||  _|| |_| |\__ \ _ | | | |"
call:print "  \___/ |_.__/ |_|   \__,_||___/(_)|_| |_|"

echo.

:: Host and protocol
set "protocol=https"
set "host=github.com:443"

:: Path to file of obfus.h
set "obfus_h_path=DosX-dev/obfus.h/raw/main/include/obfus.h"

:: Save to directory...
:: "\." - current directory (.\obfus.h)
set "output_dir=.\"

:: If error:
::  0 - stop
::  1 - continue (not recommended)
set "onerror=0"

:: Additional cURL options
set "special=--ssl-no-revoke -s"



call :process
set /a process_el=errorlevel

if "%onerror%" == "0" if "%errorlevel%" == "1" ( timeout 1 /nobreak>nul && call:log "STOPPED!" && pause>nul )

exit /b %process_el%

:process
set "curl_path=%windir%\System32\curl.exe"
call:log "Getting the latest version of obfus.h protection from %host%..."

if not exist "%curl_path%" (
    call:log "Missing cURL executable at %curl_path%!"
    exit /b 1
)

call "%curl_path%" -L %special% "%protocol%://%host%/%obfus_h_path%" -o "%output_dir%\obfus.h"

if "%errorlevel%" == "0" (
    call:log "Latest version of protection has been installed."
    exit /b 0
) else (
    call:log "An error occurred while downloading the component package."
    exit /b 1
)
goto :eof

:log
echo [%time%][pkg:obfus.h] %~1
goto :eof

:print
for /f %%a in ('"prompt $H && for %%b in (1) do rem"') do set "bs=%%a"
<nul set/p=".%bs%%~1"
echo.
goto :eof
