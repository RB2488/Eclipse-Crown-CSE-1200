@echo off
title Running Game...
cd /d "%~dp0"

echo ========================================================
echo Building and Launching Game...
echo ========================================================

"C:\Program Files (x86)\MSBuild\12.0\Bin\MSBuild.exe" maingame.vcxproj /p:Configuration=Debug /p:Platform=Win32 /nologo

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Build failed! Check the output above.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo Launching Debug\maingame.exe...
start "" "Debug\maingame.exe"
