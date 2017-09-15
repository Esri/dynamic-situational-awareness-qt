mac {
    cache()
}

TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += \
    Handheld \
    Vehicle

!android:!ios {
SUBDIRS += \
    MessageSimulator
}
