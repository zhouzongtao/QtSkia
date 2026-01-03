TEMPLATE = lib
QT += gui
# Add OpenGL module for Qt6 compatibility
qtHaveModule(opengl): QT += opengl
DEFINES += QTSKIA_LIBRARY

include($$absolute_path($$PWD/../QtSkiaGuiPublic.pri))

include($$PWD/../../uniqueDestdir.pri)
DESTDIR = $$destPath

HEADERS += \
    QSkiaOpenGLWindow.h \
    QSkiaVulkanWindow.h

SOURCES += \
    QSkiaOpenGLWindow.cpp \
    QSkiaVulkanWindow.cpp

