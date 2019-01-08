QT += quick multimedia charts
CONFIG += c++11 #qtquickcompiler

SOURCES += main.cpp \
    functions.cpp \
    pingdata.cpp \
    backend.cpp

HEADERS += \
    functions.h \
    pingdata.h \
    backend.h

RESOURCES += qml.qrc

INSTALLS += target

# icons
ICON = pinger.icns
#RC_FILE = pinger.rc
RC_ICONS = pinger.ico
