mac {
    cache()
}

TEMPLATE = subdirs

CONFIG += ordered

# check to see if the toolkit repo exists locally in any known locations
include($$PWD/Shared/build/resolvelocaltoolkit.pri)
message($$CppToolkitLocation)
!isEmpty(CppToolkitLocation) {
  SUBDIRS += $$CppToolkitLocation/ArcGISRuntimeToolkit.pro
}

SUBDIRS += \
  Handheld \
  Vehicle

!android:!ios {
SUBDIRS += \
  MessageSimulator
}
