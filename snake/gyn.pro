TEMPLATE = app
CONFIG += console
CONFIG -= c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++1y

DEFINES += YES_MAIN

SOURCES += main.cpp

HEADERS += \
    common.hpp \
    video_buffer.hpp \
    segmented_address.hpp

INCLUDEPATH += $(BOOST_ROOT)

