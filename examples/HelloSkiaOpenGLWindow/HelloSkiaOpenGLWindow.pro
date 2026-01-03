TEMPLATE = app
QT += opengl

include($$absolute_path($$PWD/../../QtSkia/QtSkiaGuiPublic.pri))
include($$absolute_path($$PWD/../Renderer/Renderer.pri))

include($$PWD/../../uniqueDestdir.pri)
DESTDIR = $$destPath
LIBS += -L$$DESTDIR -lQtSkiaGui

HEADERS += \
    SkiaGLWindow.h

SOURCES += \
    main.cpp
