mac {
    cache()
}

TEMPLATE = subdirs

CONFIG += ordered

# check to see if the toolkit repo is pulled down
exists($$PWD/../arcgis-runtime-toolkit-qt/Plugin/CppApi/ArcGISRuntimeToolkit.pro) {
  SUBDIRS += $$PWD/../arcgis-runtime-toolkit-qt/Plugin/CppApi/
}

SUBDIRS += \
    Handheld \
    Vehicle

!android:!ios {
SUBDIRS += \
    MessageSimulator
}
