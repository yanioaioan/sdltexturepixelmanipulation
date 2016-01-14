TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp
LIBS += -L/usr/lib -lSDL2 -lSDL2_image -lSDL2_ttf
INCLUDEPATH += /usr/include
