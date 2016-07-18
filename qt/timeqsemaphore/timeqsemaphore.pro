
TEMPLATE = app
TARGET   = timeqsemaphore
QT += multimedia widgets gui qml concurrent
CONFIG += console

HEADERS = timecore.hpp

SOURCES = timeqsemaphore.cpp 

QMAKE_CXXFLAGS += " -std=gnu++17 -march=native -O3 -Ofast -g -ggdb "
