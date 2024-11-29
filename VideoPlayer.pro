QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

contains(QT_ARCH, i386) {
    message("32-bit")
    DESTDIR = $${PWD}/bin32
} else {
    message("64-bit")
    DESTDIR = $${PWD}/bin64
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/Widget/selected_media_widget.cpp \
    src/Widget/show_video_widget.cpp \
    src/Widget/video_controller_widget.cpp \
    src/Widget/video_slider.cpp \
    src/app_config.cpp \
    src/main.cpp \
    src/video_player_widget.cpp \
    src/Base/function_transfer.cpp

HEADERS += \
    src/Base/define.h \
    src/Widget/selected_media_widget.h \
    src/Widget/show_video_widget.h \
    src/Widget/video_controller_widget.h \
    src/Widget/video_slider.h \
    src/app_config.h \
    src/video_player_widget.h \
    src/Base/function_transfer.h

#FORMS += \
#    src/video_player_widget.ui

INCLUDEPATH += $$PWD/src

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

include(module/VideoPlayer/VideoPlayer.pri)

include(module/DragAbleWidget/DragAbleWidget.pri)

RESOURCES += \
    resource.qrc
