# Release notes

## 1.1.5

- An "InitialLocation" config is now written out to file. This parameter is updated whenver the scene is changed (e.g. loaded from an MSPK.) This property can be used to override the startup location of the camera when the app first loads. 

## 1.1.4

- Minumum Runtime Qt SDK version is 100.9.
- Minimum Qt SDK version is Qt 5.15.0.
- Minimum Android SDK version is now 21, target SDK version is 29.
- Fixed erronious "missing file" dialog appearing on startup when opening an MSPK. This was a layer-to-json issue when saving the config.
- Reverted the "ModelSceneSymbol to PictureMarkerSymbol" change. A camera bug was was introduced because a rotation-expression can not be used in place of a heading-expression when working with an `OrbitGeoElementCameraController`. We will need to park this upgrade until an enhancement is exposed by ArcGISRuntime. 
- Fixed bug where layers were not written out to the config when the `PackageDirectory` key is missing.

## 1.1.3

- Added doc table of contents to root README.md and docs/index.md
- Renamed docs/index.md to [docs/README.md](/docs/README.md)

## 1.1.2

- Updated to use version 100.8 of the ArcGIS Runtime SDK for Qt.
- Fixed crash on RasterCell identify.

## 1.1.1

- Updated to use version 100.7 of the ArcGIS Runtime SDK for Qt.
- QML namespaces updated from `Esri.*` to `Esri.ArcGISRuntime.OpenSourceApps.*`.
- Replaced deprecated use of enum `SurfacePlacement::Draped` with `SurfacePlacement::DrapedBillboarded`.
- Replaced ModelSceneSymbol with PictureMarkerSymbol for location tracking.
- Removed deprecated "ExampleApps" icon.
- Comprehensive [app documentation](/docs/README.md) from the ArcGIS for Developers site.
- Added Troubleshooting section to root README.md.

## 1.1.0

- Updated to use version 100.4 of the ArcGIS Runtime SDK for Qt.
- Added MrSid raster layer support.
- Added KML layer support.
- Fixed issue with location display.
- The 360 viewshed now utilizes the new official runtime support.

## 1.0.0

This is the first release of the Dynamic Situational Awareness app for Qt.
