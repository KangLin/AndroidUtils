
versionAtMost(QT_VERSION, 5.4.0) : error("Android: Qt version must greater than or equal to 5.4.0")

QT += androidextras gui

INCLUDEPATH += $$PWD
LIBS += -ljnigraphics

SOURCES += \ 
    $$PWD/AndroidDirectory.cpp \
    $$PWD/AndroidUtils.cpp \
    $$PWD/Notification.cpp \
    $$PWD/NotificationNative.cpp

HEADERS += \ 
    $$PWD/AndroidDirectory.h \
    $$PWD/AndroidUtils.h \
    $$PWD/Notification.h \
    $$PWD/NotificationNative.h

equals(BUILD_TEST, 1){
    DEFINES += BUILD_TEST
    SOURCES += $$PWD/Test.cpp
    HEADERS += $$PWD/Test.h
}
