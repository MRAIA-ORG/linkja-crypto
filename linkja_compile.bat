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
cd "%ProgramFiles%\Linkja\linkja-crypto-master"
del "%ProgramFiles%\Linkja\linkja-crypto-master\CMakeCache.txt"
del "%ProgramFiles%\Linkja\linkja-crypto-master\src\linkjacrypto.exp"
del "%ProgramFiles%\Linkja\linkja-crypto-master\src\linkjacrypto.lib"
del "%ProgramFiles%\Linkja\linkja-crypto-master\src\linkjacrypto.dll"
del "%ProgramFiles%\Linkja\linkja-crypto-master\src\linkjacrypto.dll.manifest"
del "%ProgramFiles%\Linkja\linkja-crypto-master\src\include\linkja_secret.h"
echo If you see error messages for the above six file deletions, do not worry, as the files do not yet exist to be deleted.
cmake -DCMAKE_BUILD_TYPE=Release -G "NMake Makefiles" .
nmake clean
nmake
echo If "Built target linkjacrypto" is shown above, the build completed successfully. Go to the Linkja Crypto library folder's /src/ subfolder, and distribute the linkjacrypto.dll file as needed. If the build failed, please contact Linkja Support for assistance.
pause