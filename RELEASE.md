# Release notes

## 1.2.0

- Minumum Runtime Qt SDK version is 100.9.
- Minimum Qt SDK version is Qt 5.15.0.
- Minimum Android SDK version is now 21, target SDK version is 29.
- Changed expected location of DSA files to match the changes Android
  has made to scoped storage.
  When previously data files needed copied to `/sdcard/ArcGIS/Runtime/DSA/Data`,
  now data must be copied to `<AppDataLocation>/ArcGIS/Runtime/DSA/Data`.
  For example, with the DSA Vehicle app, the data is expected to be located
  at: `/sdcard/Android/data/com.esri.arcgisruntime.opensourceapps.DSA_Vehicle_Qt/files/ArcGIS/Runtime/DSA/Data`.

## 1.1.2

- Updated to use version 100.8 of the ArcGIS Runtime SDK for Qt.
- Fixed crash on RasterCell identify.

## 1.1.1

- Updated to use version 100.7 of the ArcGIS Runtime SDK for Qt.
- QML namespaces updated from `Esri.*` to `Esri.ArcGISRuntime.OpenSourceApps.*`.
- Replaced deprecated use of enum `SurfacePlacement::Draped` with `SurfacePlacement::DrapedBillboarded`.
- Replaced ModelSceneSymbol with PictureMarkerSymbol for location tracking.
- Removed deprecated "ExampleApps" icon.
- Comprehensive [app documentation](/docs/index.md) from the ArcGIS for Developers site.
- Added Troubleshooting section to README.md.

## 1.1.0

- Updated to use version 100.4 of the ArcGIS Runtime SDK for Qt.
- Added MrSid raster layer support.
- Added KML layer support.
- Fixed issue with location display.
- The 360 viewshed now utilizes the new official runtime support.

## 1.0.0

This is the first release of the Dynamic Situational Awareness app for Qt.
