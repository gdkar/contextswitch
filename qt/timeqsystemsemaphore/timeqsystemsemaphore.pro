
TEMPLATE = app
TARGET   = timeqsystemsemaphore
QT += multimedia widgets gui qml concurrent
CONFIG += console

HEADERS = timecore.hpp

SOURCES = timeqsystemsemaphore.cpp 

QMAKE_CXXFLAGS += " -std=gnu++17 -march=native -O3 -Ofast -g -ggdb "
