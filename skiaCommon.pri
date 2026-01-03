isEmpty(skiaCommond) {
    skiaCommon = 1
    QT += core gui

    # Try to include prebuilt Skia configuration from parent directory
    exists($$PWD/use_prebuilt_skia.pri) {
        include($$PWD/use_prebuilt_skia.pri)
    }

    CONFIG += c++17

    # Qt6 already adds -utf-8 flag, so don't add -source-charset:utf-8
    # to avoid MSVC conflict
    msvc {
        !equals(QT_MAJOR_VERSION, 6) {
            QMAKE_CFLAGS += -source-charset:utf-8
            QMAKE_CXXFLAGS += -source-charset:utf-8
        }
        # Use static runtime (/MT) to match prebuilt Skia library
        # This must be set at the CONFIG level to override Qt defaults
        CONFIG -= dynamic
        CONFIG += static
        # Force /MT flag to override Qt's /MD
        QMAKE_CXXFLAGS_RELEASE = -O2 -MT -std:c++17 -utf-8
        QMAKE_CXXFLAGS_DEBUG = -Zi -MTd -std:c++17 -utf-8
        QMAKE_CFLAGS_RELEASE = -O2 -MT -utf-8
        QMAKE_CFLAGS_DEBUG = -Zi -MTd -utf-8
    }

    # Add Windows SDK shared include directory for OpenGL headers
    win32 {
        WIN_SDK_SHARED = $$(WindowsSDKVersion)
        !isEmpty(WIN_SDK_SHARED) {
            INCLUDEPATH += C:/Program\ Files\ \(x86\)/Windows\ Kits/10/include/$$WIN_SDK_SHARED/shared
        } else {
            # Fallback to common Windows 10 SDK versions
            exists(C:/Program\ Files\ \(x86\)/Windows\ Kits/10/include/10.0.26100.0/shared) {
                INCLUDEPATH += C:/Program\ Files\ \(x86\)/Windows\ Kits/10/include/10.0.26100.0/shared
            }
        }
        # Link OpenGL library for WGL functions
        LIBS += -lopengl32
        # Link DirectWrite and other Windows libraries for font rendering
        LIBS += -ldwrite -luser32 -lgdi32 -lole32 -loleaut32 -luuid
    }

    include($$PWD/skiaBuild/buildConfig/buildConfig.pri)
    include($$PWD/uniqueDestdir.pri)

    # Check if prebuilt Skia configuration is loaded
    defined(SKIA_PREBUILT_PATH, var) {
        # Use prebuilt Skia from E:\Repository\skia
        SKIA_SRC_PATH=$$SKIA_PREBUILT_PATH
        SKIA_LIB_PATH=$$SKIA_PREBUILT_OUT
        SKIA_INCLUDE_PATH=$$SKIA_SRC_PATH/include
        message("Using prebuilt Skia from: $$SKIA_LIB_PATH")
    } else {
        # Use default Skia path (3rdparty/skia)
        SKIA_OUT_PATH=$$destPath
        SKIA_SRC_PATH=$$absolute_path($$PWD/3rdparty/skia)
        SKIA_LIB_PATH=$$SKIA_OUT_PATH
        SKIA_INCLUDE_PATH=$$SKIA_SRC_PATH/include
        message("Using default Skia from: $$SKIA_LIB_PATH")
    }

    # For static builds, define QTSKIA_STATIC to disable dllimport/dllexport
    if($$QtSkia_Static_Build) {
        DEFINES += QTSKIA_STATIC
    } else {
        # Only define SKIA_DLL for shared builds
        DEFINES += SKIA_DLL
    }
    DEFINES += QT_NO_FOREACH

    INCLUDEPATH +=$$SKIA_SRC_PATH $$SKIA_INCLUDE_PATH

    DEPENDPATH +=$$SKIA_LIB_PATH

    if($$QtSkia_Static_Build) {
        LIBS += -L$$SKIA_LIB_PATH -lskia
        message("static lib:skia")
    } else {
        win32{
            LIBS += -L$$SKIA_LIB_PATH -lskia.dll
            message("shared lib: skia.dll")
        } else {
            LIBS += -L$$SKIA_LIB_PATH -lskia
            message("shared lib: skia")
        }
    }
}
