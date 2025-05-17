@echo off

pushd %~dp0\..\

if "%~1"=="" (
    echo Usage: generate.bat ^<TARGET^>
    echo Available targets: vs, make
    exit /b 1
)

set TARGET=%~1
set ACTION=
set PREMAKE=third-party\premake\premake5.exe
set PREMAKE_SCRIPT=scripts\premake\premake5.lua

if /i "%TARGET%"=="vs" (
    set ACTION=vs2022
) else if /i "%TARGET%"=="make" (
    set ACTION=gmake
) else (
    echo Invalid target: %TARGET%
    echo Available targets: vs, make
    exit /b 1
)


%PREMAKE% --file=%PREMAKE_SCRIPT% %ACTION%

popd