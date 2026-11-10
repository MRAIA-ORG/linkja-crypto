@echo off
goto check_Permissions

:check_Permissions
    echo Administrative permissions required. Detecting permissions...
    net session >nul 2>&1
    if %errorLevel% == 0 (
        echo Success: Administrative permissions confirmed.
    ) else (
        echo Failure: Current permissions inadequate.
        pause
        EXIT /B %ERRORLEVEL%
    )

SET PATH=%PATH%;"%ProgramFiles%\OpenSSL-Win64\bin"

:: 1. Check if the CURRENT folder is named correctly
FOR %%I IN ("%~dp0.") DO SET "currentDirName=%%~nxI"

IF /I "%currentDirName%"=="linkja-crypto" (
    SET "targetDir=%~dp0"
    goto :Compile
)
IF /I "%currentDirName%"=="linkja-crypto-master" (
    SET "targetDir=%~dp0"
    goto :Compile
)

:: 2. If not, check the default Program Files locations
IF EXIST "%ProgramFiles%\Linkja\linkja-crypto-master" (
    SET "targetDir=%ProgramFiles%\Linkja\linkja-crypto-master"
    goto :Compile
)
IF EXIST "%ProgramFiles%\Linkja\linkja-crypto" (
    SET "targetDir=%ProgramFiles%\Linkja\linkja-crypto"
    goto :Compile
)

:: 3. If none of the above, fail
echo Failure: No linkja-crypto or linkja-crypto-master directory found.
pause
exit /b 1

:Compile
echo Switching to: "%targetDir%"
cd /d "%targetDir%"

:: Clean up old files using relative paths (cleaner)
del "CMakeCache.txt" 2>nul
del "src\linkjacrypto.exp" 2>nul
del "src\linkjacrypto.lib" 2>nul
del "linkjacrypto.dll" 2>nul
del "linkjacrypto.dll.manifest" 2>nul
del "src\include\linkja_secret.h" 2>nul

echo If you see error messages for the above six file deletions, do not worry.

:: Run Build
cmake -DCMAKE_BUILD_TYPE=Release -G "NMake Makefiles" .
nmake clean
nmake

echo If "Built target linkjacrypto" is shown above, the build completed successfully.
pause