@echo off
setlocal enabledelayedexpansion

pushd %~dp0\..\

if "%~1"=="" (
    echo Usage: generate.bat ^<TARGET^> [COMPILER]
    echo Available targets: vs, make
    echo Available compilers:
    echo   For vs: msc, clang
    echo   For make: gcc, clang
    exit /b 1
)

set TARGET=%~1
set COMPILER=%~2
set ACTION=
set PREMAKE=third-party\premake\premake5.exe
set PREMAKE_SCRIPT=scripts\premake\premake5.lua
set CC_OPTION=

if /i "%TARGET%"=="vs" (
    set ACTION=vs2022
    set DEFAULT_COMPILER=msc
    set VALID_COMPILERS=msc clang
) else if /i "%TARGET%"=="make" (
    set ACTION=gmake
    set DEFAULT_COMPILER=gcc
    set VALID_COMPILERS=gcc clang
) else (
    echo Invalid target: %TARGET%
    echo Available targets: vs, make
    exit /b 1
)

if not "%COMPILER%"=="" (
    set IS_VALID=0
    for %%C in (%VALID_COMPILERS%) do (
        if /i "%%C"=="%COMPILER%" set IS_VALID=1
    )
    if !IS_VALID!==0 (
        echo Invalid compiler for %TARGET%: %COMPILER%
        echo Available compilers: %VALID_COMPILERS%
        exit /b 1
    )
    set CC_OPTION=--cc=%COMPILER%
) else (
    set CC_OPTION=--cc=%DEFAULT_COMPILER%
)

%PREMAKE% --file=%PREMAKE_SCRIPT% %ACTION% %CC_OPTION%

popd