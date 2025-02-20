# Overview

- [Dynamic Situational Awareness](#dynamic-situational-awareness)
- [Release notes](#release-notes)
- [Detailed documentation](#detailed-documentation)
- [Quick start](#quick-start)
- [Prerequisites](#prerequisites)
   - [Build dependency](#build-dependency)
   - [Data](#data)
- [Resources](#resources)
- [Pre-built apps](#pre-built-apps)
- [Issues](#issues)
- [Troubleshooting](#troubleshooting)
   - [Military Symbols are not displaying after migrating to version 1.1.1.](#military-symbols-are-not-displaying-after-migrating-to-version-111)
- [Contributing](#contributing)
- [Licensing](#licensing)

---

## Dynamic Situational Awareness

When the operational landscape changes frequently, then rapid, accurate, purpose-built, mission-focused communication is key to success. This is the working domain of the Dynamic Situational Awareness Open Source App (DSA) built using [ArcGIS Maps SDK for Qt](https://developers.arcgis.com/qt). DSA enables physically distributed teams to work as one, maintaining current status on teammates while exploring and illuminating the operational landscape.

DSA is an **Open Source App** intended and designed for the developer who wants to build their own custom field operation apps. Built using the [ArcGIS Maps SDK for Qt](https://developers.arcgis.com/qt), DSA highlights developer best practices for the specific workflows for in-vehicle (mounted) and field (handheld/dismounted) teams who need to dynamically understand their environment and the situation around them.

The DSA apps do not require access to a server. All the data and processing are on the device itself. Display of each local data source may be toggled on or off. Teammates share information such as own location, observation reports and markup across the peer-to-peer network (UDP). The emphasis is on collaboration with other teammates. These networks are not connected to the Internet, are not high-bandwidth, and are not constantly connected - a situation commonly encountered in field operations. This communication network topology is unlike more traditional ArcGIS Maps SDK apps that leverage the Internet and web servers for communication or web services for some functionality.


## Release notes

Please check the [release notes](./RELEASE.md) for a detailed description of the enhancements and fixes for each release of the DSA application.

## Detailed documentation

Read the [docs](./docs/README.md) for a detailed explanation of the application, including its architecture and how it leverages the ArcGIS platform, as well as how you can begin using the app right away.

## Quick start

The repo contains [Qt](http://qt.io) projects for dynamic situational awareness applications that can be run from within the Qt Creator IDE.

If you just want to grab the latest code and build the project, these are the quick steps:

 * `git clone https://github.com/Esri/dynamic-situational-awareness-qt.git`
 * `cd dynamic-situational-awareness-qt`
 * `git clone --branch 200.6.0.final https://github.com/Esri/arcgis-maps-sdk-toolkit-qt.git`
 * Open `DSA.pro` with Qt Creator, configure the projects and build.

**Note:** There is a file path length limit when deploying the DSA apps to Android from Windows. The simplest way to workaround this is to shorten the folder name of the "Shadow Build Directory" in the Qt Creator Project settings.

## Prerequisites

* ArcGIS Maps SDK for Qt 200.6
* Qt SDK 6.5.6 or higher
* Qt Creator
* [ArcGIS Maps SDK for Qt Toolkit](https://github.com/Esri/arcgis-maps-sdk-toolkit-qt.git)
* For Windows: Microsoft Visual C++ Compiler 16.1.6
* For more information, please visit the [System Requirements](https://developers.arcgis.com/qt/system-requirements/system-requirements-for-200-6/) page.

### Build dependency

Set up the [ArcGIS Maps SDK for Qt Toolkit](https://github.com/Esri/arcgis-maps-sdk-toolkit-qt.git) as a build dependency.

```
# from the base of the repo
git clone --branch 200.6.0.final https://github.com/Esri/arcgis-maps-sdk-toolkit-qt.git
```

### Data

The DSA app works with offline data. You can retrieve [demo data available for Monterey, California](http://www.arcgis.com/home/item.html?id=82ce2d85e21c4326bc072d441b636e5e). Details for where to place the data are provided in the demo data's description. Alternatively, you can define your own data by modifying the app's configuration file.

## Resources

* [ArcGIS Maps SDK for Qt Resource Center](https://developers.arcgis.com/qt/latest/)
* [Qt and QML](http://www.qt.io/)
* [ArcGIS Blog](http://blogs.esri.com/esri/arcgis/)
* [twitter@esri](https://x.com/esri)

## Pre-built apps

The compiled versions of the DSA app are made available for you to conveniently try out the app, and are not officially supported. Please note that these compiled versions of the app should not be used in production environments.

- [Android](https://www.arcgis.com/home/item.html?id=76fdb565194d4dcc86bdd301131932ea)
- [Windows](https://www.arcgis.com/home/item.html?id=6b32eec2db784d0da9e9d5b18ccefb76)

## Issues

Find a bug or want to request a new feature?  Please let us know by submitting an issue.

## Troubleshooting

### Military Symbols are not displaying after migrating to version 1.1.1.

DSA now requires additional `*.stylx` files in the `styles/arcade` subdirectory of your DSA resource data. The structure
should look like this:

* `DSA/ResourceData/styles/arcade/mil2525c.stylx`
* `DSA/ResourceData/styles/arcade/mil2525d.stylx`

To quickly obtain these additional `*.stylx` files, we provide these in the [demo data](http://www.arcgis.com/home/item.html?id=82ce2d85e21c4326bc072d441b636e5e).

## Contributing

Esri welcomes contributions from anyone and everyone. Please see our [guidelines for contributing](https://github.com/esri/contributing).

## Licensing

Copyright 2018 Esri

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at


   http://www.apache.org/licenses/LICENSE-2.0


Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.


A copy of the license is available in the repository's [LICENSE](LICENSE) file

[Open Source App page]: http://developers.arcgis.com/example-apps/dsa-app-qt/?utm_source=github&utm_medium=web&utm_campaign=example_apps_dsa_qt
