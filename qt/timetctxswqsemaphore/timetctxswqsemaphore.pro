TEMPLATE = app
TARGET   = timetctxswqsemaphore
QT += multimedia widgets gui qml concurrent
CONFIG += console

HEADERS = timecore.hpp  \
          test_qsemaphore.hpp

SOURCES = timetctxswqsemaphore.cpp 

QMAKE_CXXFLAGS += " -std=gnu++17 -march=native -O3 -Ofast -g -ggdb "
