CONFIG += c++17

INCLUDEPATH += $$PWD/src

HEADERS += \
    $$PWD/src/Mutex/cond.h \
    $$PWD/src/Mutex/mutex.h \
    $$PWD/src/EventHandle/video_player_event_handle.h \
    $$PWD/src/VideoPlayer/Video/video_frame.h \
    $$PWD/src/VideoPlayer/video_player.h \
    $$PWD/src/types.h \

SOURCES +=  \
    $$PWD/src/Mutex/cond.cpp \
    $$PWD/src/Mutex/mutex.cpp \
    $$PWD/src/EventHandle/video_player_event_handle.cpp \
    $$PWD/src/VideoPlayer/Video/video_frame.cpp \
    $$PWD/src/VideoPlayer/video_player.cpp \
    $$PWD/src/types.cpp \
    $$PWD/src/main.cpp

### lib ### Begin
    include($$PWD/lib/lib.pri)
### lib ### End
