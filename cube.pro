QT       += core gui widgets serialport

TARGET = cube
TEMPLATE = app

SOURCES += main.cpp \
    serialport.cpp

SOURCES += \
    mainwidget.cpp \
    geometryengine.cpp

HEADERS += \
    mainwidget.h \
    geometryengine.h \
    serialport.h

LIBS += -LD:\install\qt5.8\5.8\mingw53_32\bin \
        -LD:\install\qt5.8\5.8\mingw53_32\lib \
   -lQt5SerialPortd

RESOURCES += \
    shaders.qrc \
    textures.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/cube
INSTALLS += target
