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

set "QT_ROOT=C:\Qt6.8.2\6.8.2"
set "QT_ANDROID_BIN=%QT_ROOT%\android_arm64_v8a\bin"
set "QT_DEPLOY_TOOL=%QT_ROOT%\mingw_64\bin\androiddeployqt.exe"
set "MAKE_EXE=%USERPROFILE%\AppData\Local\Android\Sdk\ndk\28.0.12433566\prebuilt\windows-x86_64\bin\make.exe"
set "ANDROID_NDK_ROOT=%USERPROFILE%\AppData\Local\Android\Sdk\ndk\28.0.12433566"
set "ANDROID_PLATFORM=android-34"

:: =============================
:: Workspace & Paths
:: =============================
cd /d "%~dp0\.." || exit /b 1
set "WORKSPACE=%cd%"
set "BUILD_DIR=%WORKSPACE%\build"
set "ANDROID_BUILD_DIR=%BUILD_DIR%\android-build"
set "PRO_FILE=%WORKSPACE%\%APP_NAME%\%APP_NAME%.pro"
set "DEPLOY_JSON=%BUILD_DIR%\android-DSA_%APP_NAME%_Qt-deployment-settings.json"

:: =============================
:: Prepare Build Directory
:: =============================
echo [INFO] Cleaning build directory...
if exist "%BUILD_DIR%" rmdir /S /Q "%BUILD_DIR%"
mkdir "%BUILD_DIR%"
cd "%BUILD_DIR%" || exit /b 1

:: =============================
:: Run QMake and Make
:: =============================
echo [INFO] Running qmake...
call "%QT_ANDROID_BIN%\qmake.bat" ^
    "%PRO_FILE%" ^
    -spec android-clang ^
    CONFIG+=debug CONFIG+=qml_debug || exit /b 1

echo [INFO] Building project...
"%MAKE_EXE%" -f Makefile qmake_all || exit /b 1
"%MAKE_EXE%" INSTALL_ROOT="%ANDROID_BUILD_DIR%" install -j12 || exit /b 1

:: =============================
:: Deploy with androiddeployqt
:: =============================
echo [INFO] Running androiddeployqt...
"%QT_DEPLOY_TOOL%" ^
    --input "%DEPLOY_JSON%" ^
    --output "%ANDROID_BUILD_DIR%" ^
    --android-platform "%ANDROID_PLATFORM%" ^
    --jdk "%JAVA_HOME%" ^
    --gradle || exit /b 1

:: =============================
:: Copy and Rename APK
:: =============================
set "APK_NAME=%ANDROID_BUILD_DIR%\build\outputs\apk\debug\android-build-debug.apk"
set "DEPLOY_DIR=C:\applications\DSA"
set "RENAMED_APK=%DEPLOY_DIR%\DSA_%APP_NAME%.apk"

echo [INFO] Copying APK to deployment directory...

if exist "%APK_NAME%" (
    if not exist "%DEPLOY_DIR%" mkdir "%DEPLOY_DIR%"
    copy /Y "%APK_NAME%" "%RENAMED_APK%" >nul
    echo [SUCCESS] APK copied to: %RENAMED_APK%
) else (
    echo [ERROR] APK not found at: %APK_NAME%
)

echo [SUCCESS] Android build and deployment completed.
exit /b 0
