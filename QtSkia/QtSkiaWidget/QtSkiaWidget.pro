TEMPLATE = lib
QT += widgets
# Add OpenGL modules for Qt6 compatibility
qtHaveModule(opengl): QT += opengl
qtHaveModule(openglwidgets): QT += openglwidgets
DEFINES += QTSKIA_LIBRARY

include($$PWD/../QtSkiaWidgetPublic.pri)

include($$PWD/../../uniqueDestdir.pri)
DESTDIR = $$destPath

HEADERS += \
    QSkiaWidget.h \
    QSkiaOpenGLWidget.h

SOURCES += \
    QSkiaWidget.cpp \
    QSkiaOpenGLWidget.cpp
