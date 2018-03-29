mac {
    cache()
}

TEMPLATE = subdirs

CONFIG += ordered

# check to see if the toolkit repo exists locally in any known locations
include($$PWD/Shared/build/resolvelocaltoolkit.pri)
!isEmpty(CppToolkitLocation):!contains(SUBDIRS, $$CppToolkitLocation) {
  SUBDIRS += $$CppToolkitLocation
}

SUBDIRS += \
  Handheld \
  Vehicle

!android:!ios {
SUBDIRS += \
  MessageSimulator
}
