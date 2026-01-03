TEMPLATE = subdirs

# Load prebuilt Skia configuration if it exists at parent level
exists(../use_prebuilt_skia.pri) {
    include(../use_prebuilt_skia.pri)
}

SUBDIRS += \
    QtSkiaWidget \
    QtSkiaGui \
    QtSkiaQuick


