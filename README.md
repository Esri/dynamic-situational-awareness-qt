# Overview
The repo contains [Qt](http://qt.io) projects for dynamic situational awareness applications that can be run from within the Qt Creator IDE.

# Prerequisites
* ArcGIS Runtime SDK for Qt 100.2.1
* Qt SDK 5.9.2 or higher
* Qt Creator
* ArcGIS Runtime SDK for Qt C++ Plugin [(details)](https://github.com/Esri/arcgis-runtime-toolkit-qt)
* For Windows: Microsoft Visual C++ Compiler 14.0 
* For more information, please visit the [System Requirements](https://developers.arcgis.com/qt/quartz/qml/guide/arcgis-runtime-sdk-for-qt-system-requirements.htm) page.

## Fork the repo
If you haven't already, fork [the repo](https://github.com/ArcGIS/dynamic-situational-awareness-qt/fork).

## Clone the repo

### Command line Git
[Clone the ArcGIS Qt SDK Samples](https://help.github.com/articles/fork-a-repo#Step-2-clone-your-fork)

Open your terminal, navigate to your working directory, use ```git clone``` to get a copy of the repo.

```
# Clone your fork of the repository into the current directory in terminal
$ git clone https://github.com/YOUR-USERNAME/dynamic-situational-awareness-qt.git
```

## Configure remote upstream for your fork
To sync changes you make in a fork with this repository, you must configure a remote that points to the upstream repository in Git.

- Open a terminal (Mac users) or command prompt (Windows & Linux users)
- List the current configured remote repository for your fork

```
$ git remote -v
origin	https://github.com/YOUR_USERNAME/dynamic-situational-awareness-qt.git (fetch)
origin	https://github.com/YOUR_USERNAME/dynamic-situational-awareness-qt.git (push)
```

- Specify a new remote upstream repository

```
$ git remote add upstream https://github.com/ArcGIS/dynamic-situational-awareness-qt.git
```

- Verify the new upstream repository

```
$ git remote -v

origin	https://github.com/YOUR_USERNAME/dynamic-situational-awareness-qt.git (fetch)
origin	https://github.com/YOUR_USERNAME/dynamic-situational-awareness-qt.git (push)
upstream https://github.com/ArcGIS/dynamic-situational-awareness-qt.git (fetch)
upstream https://github.com/ArcGIS/dynamic-situational-awareness-qt.git (push)
```

### Sync your fork
Once you have set up a remote upstream you can keep your fork up to date with our samples repository by syncing your fork.

- Open a terminal (Mac users) or command prompt (Windows & Linux users)
- Change to the current working directory of your local repository
- Fetch the branches and commits from the upstream repository.  Commits to ```master``` will be stored in a local branch, ```upstream/master```.

```
$ git fetch upstream
```

- Check out your forks local ```master``` branch

```
$ git checkout master
```

- Merge changes from ```upstream/master``` into  your local ```master``` branch which syncs your forks ```master``` branch with our samples repository.

```
$ git merge upstream/master
```

### Setup the [ArcGIS Runtime SDK C++ Toolkit](https://github.com/Esri/arcgis-runtime-toolkit-qt) as a build dependency.

- This is an optional step but allows you to build with the latest version of the toolkit

```
# from the base of the repo
git clone https://github.com/Esri/arcgis-runtime-toolkit-qt arcgis-runtime-toolkit-qt
```

- To avoid any conflicts with the toolkit library from the SDK install, set `CONFIG += ToolkitBuildUsePrefix` in the toolkit and DSA projects. The setting already exists in the projects but is commented out by default.

## Open a project file in Qt Creator
Start Qt Creator. When the IDE opens to the Welcome screen, click on the **Open Project** button and browse to a project file (.pro) within your forked repo location.
Configure the project and run the sample.

## Resources

* [ArcGIS Runtime QtSDK Resource Center](https://developers.arcgis.com/qt/latest/)
* [Qt and QML](http://www.qt.io/)
* [ArcGIS Blog](http://blogs.esri.com/esri/arcgis/)
* [twitter@esri](http://twitter.com/esri)

## Issues
Find a bug or want to request a new feature?  Please let us know by submitting an issue.

## Contributing
Esri welcomes contributions from anyone and everyone. Please see our [guidelines for contributing](https://github.com/esri/contributing).

## Licensing
Copyright 2017 Esri


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
