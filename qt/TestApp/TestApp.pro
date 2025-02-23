TEMPLATE = app

QT += qml quick

CONFIG += c++17

SOURCES += TestApp.cpp \
    touchstonedata.cpp

RESOURCES += TestApp.qrc

target.path = build
INSTALLS += target

HEADERS += \
    touchstonedata.h
