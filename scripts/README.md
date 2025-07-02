# Build Instructions


1. Modify the following variables in the script based on your local setup:

- QT_ROOT : Path to your Qt installation
- ANDROID_NDK_ROOT : Path to your Android NDK
- ANDROID_PLATFORM : Android API level (e.g., `android-34`)


2. Clone the Dynamic Situational Awareness (DSA) repository

```
git clone https://github.com/Esri/dynamic-situational-awareness-qt.git
```

3. Clone the ArcGIS Maps SDK for Qt Toolkit repository

```
cd dynamic-situational-awareness-qt
git clone https://github.com/Esri/arcgis-maps-sdk-toolkit-qt.git
```

4. Navigate to the scripts directory and run the appropriate build script, specifying the application type (Vehicle or Handheld):

- For windows:
```
build_windows.bat <application>
```

- For android:

```
build_android.bat <application>
```
