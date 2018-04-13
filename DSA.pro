#-------------------------------------------------
#  Copyright 2018 ESRI
#
#  All rights reserved under the copyright laws of the United States
#  and applicable international laws, treaties, and conventions.
#
#  You may freely redistribute and use this sample code, with or
#  without modification, provided you include the original copyright
#  notice and use restrictions.
#
#  See the Sample code usage restrictions document for further information.
#-------------------------------------------------

mac {
    cache()
}

TEMPLATE = subdirs

CONFIG += ordered

# check to see if the toolkit repo exists locally in any known locations
include($$PWD/Shared/build/resolvelocaltoolkit.pri)
message($$CppToolkitLocation)
!isEmpty(CppToolkitLocation) {
  SUBDIRS += $$CppToolkitLocation/ArcGISRuntimeToolkit_StaticLib.pro
}

SUBDIRS += \
  Handheld \
  Vehicle

!android:!ios {
SUBDIRS += \
  MessageSimulator
}
