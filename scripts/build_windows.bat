@echo off
setlocal

:: =============================
:: Configuration
:: =============================
set "APP_NAME=%~1"
if "%APP_NAME%"=="" (
    echo [ERROR] Application name argument missing.
    exit /b 1
)

set "VCVARS_BAT=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
set "QT_BIN=C:\Qt6.8.2\6.8.2\msvc2022_64\bin"
set "JOM_EXE=C:\Qt6.8.2\Tools\QtCreator\bin\jom\jom.exe"
set "ARC_GIS_SDK=C:\Program Files\ArcGIS SDKs\Qt300.0.0"
set "DEPLOY_DIR=C:\applications\DSA"

:: =============================
:: Workspace & Paths
:: =============================
cd /d "%~dp0\.." || exit /b 1
set "WORKSPACE=%cd%"
set "BUILD_DIR=%WORKSPACE%\build"
set "PRO_FILE=%WORKSPACE%\%APP_NAME%\%APP_NAME%.pro"
set "APP_EXEC=DSA_%APP_NAME%_Qt.exe"
set "TARGET_DIR=%DEPLOY_DIR%\DSA_%APP_NAME%"

:: =============================
:: Setup Environment
:: =============================
echo [INFO] Initializing MSVC environment...
call "%VCVARS_BAT%" || exit /b 1

:: =============================
:: Prepare Build Directory
:: =============================
echo [INFO] Cleaning build directory...
if exist "%BUILD_DIR%" rmdir /S /Q "%BUILD_DIR%"
mkdir "%BUILD_DIR%"
cd "%BUILD_DIR%" || exit /b 1

:: =============================
:: Run qmake and jom
:: =============================
echo [INFO] Running qmake...
"%QT_BIN%\qmake.exe" ^
    "%PRO_FILE%" ^
    -r -spec win32-msvc ^
    CONFIG+=release || exit /b 1

echo [INFO] Running jom build...
"%JOM_EXE%" || exit /b 1

:: =============================
:: Prepare Output Directory
:: =============================
echo [INFO] Preparing deployment directory...
if not exist "%TARGET_DIR%" mkdir "%TARGET_DIR%"

:: =============================
:: Copy Executable and ArcGIS Files
:: =============================
echo [INFO] Searching for built executable...
set "FOUND_EXEC="
for /R "%BUILD_DIR%\release" %%F in (%APP_EXEC%) do (
    set "FOUND_EXEC=%%~fF"
    goto :deploy
)

echo [ERROR] Executable not found: %APP_EXEC%
exit /b 1

:deploy
echo [INFO] Copying executable to %TARGET_DIR%...
copy "%FOUND_EXEC%" "%TARGET_DIR%\" >nul

echo [INFO] Copying ArcGIS runtime files...
copy "%ARC_GIS_SDK%\sdk\windows\x64\bin\EsriCommonQt.dll" "%TARGET_DIR%\" >nul
copy "%ARC_GIS_SDK%\sdk\windows\x64\bin\runtimecore.dll" "%TARGET_DIR%\" >nul
xcopy /E /I /Y "%ARC_GIS_SDK%\sdk\windows\x64\shaders" "%TARGET_DIR%\shaders" >nul

:: =============================
:: Run windeployqt
:: =============================
echo [INFO] Running windeployqt...
"%QT_BIN%\windeployqt6.exe" "%TARGET_DIR%\%APP_EXEC%" --qmldir "%WORKSPACE%" --websockets

echo [SUCCESS] Windows build and deployment completed.
exit /b 0
