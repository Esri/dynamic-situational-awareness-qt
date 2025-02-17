# Release notes

## 2.0.0

- Minimum ArcGIS Maps SDK for Qt version is 200.6
- Minimum Qt SDK version is 6.5.6
- Replaced instances of TaskWatcher with QFuture ([deprecated](https://developers.arcgis.com/qt/release-notes/prior-releases/release-notes-for-200-3/#deprecations) in the ArcGIS Maps SDK for Qt at version 200.3)
- Mobile platforms now use local app storage for DSA data instead of shared storage (legacy storage `/sdcard` no longer available on recent versions of Android)
- Works with latest capabilities in Native SDKs: Dynamic Entities. Less custom code to handle real-time feeds.
- New "Configurations" tool that will download the default demo dataset from ArcGIS Online for you. Can manage multiple configurations/packages in the app and switch between them. Check the documentation [here](/docs/DataManagement.md) for instructions on migrating data used in previous versions of DSA.

## 1.1.8

- Minimum Runtime Qt SDK version is 100.15 LTS.
- Minimum Qt SDK version is Qt 5.15.2.
- Removes usages of deprecated functionality in preparation for Qt 6 and Runtime 200.

## 1.1.7

- Update links within [docs/README.md](/docs/README.md) to direct to new pages

## 1.1.6

- Minimum Runtime Qt SDK version is 100.10.
- Minimum Qt SDK version is Qt 5.15.1.
- DSA now running on the new toolkit architecture.
- Fixed bug where Markups were no longer shared.

## 1.1.5

- An "InitialLocation" config is now written out to file. This parameter is updated whenever the scene is changed (e.g. loaded from an MSPK.) This property can be used to override the startup location of the camera when the app first loads. 

## 1.1.4

- Minimum Runtime Qt SDK version is 100.9.
- Minimum Qt SDK version is Qt 5.15.0.
- Minimum Android SDK version is now 21, target SDK version is 29.
- Fixed erroneous "missing file" dialog appearing on startup when opening an MSPK. This was a layer-to-json issue when saving the config.
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
