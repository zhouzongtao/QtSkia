TEMPLATE = subdirs

# Load prebuilt Skia configuration if it exists
exists(use_prebuilt_skia.pri) {
    include(use_prebuilt_skia.pri)
    SUBDIRS += \
        QtSkia \
        examples \
        tests
    message("Using prebuilt Skia, skipping skiaBuild")
} else {
    SUBDIRS += \
        skiaBuild \
        QtSkia \
        examples \
        tests
    message("Using skiaBuild to build Skia")
}
CONFIG += ordered

OTHER_FILES += \
    skiacommon.pri \
    uniqueDestdir.pri \
    *.md \
    LICENSE \
    .clang-format \
    .qmake.conf \
    syncSkia* \
    .github/workflows/*.yml
