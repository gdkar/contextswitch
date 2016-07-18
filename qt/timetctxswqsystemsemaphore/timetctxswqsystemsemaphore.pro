TEMPLATE = app
TARGET   = timetctxswqsystemsemaphore
QT += multimedia widgets gui qml concurrent
CONFIG += console

HEADERS = timecore.hpp  \
          test_systemsemaphore.hpp

SOURCES = timetctxswqsystemsemaphore.cpp 

QMAKE_CXXFLAGS += " -std=gnu++17 -march=native -O3 -Ofast -g -ggdb "
