#-------------------------------------------------
#
# Project created by QtCreator 2014-05-19T20:05:08
#
#-------------------------------------------------

TARGET = bsaExtractor
TEMPLATE = lib

contains(QT_VERSION, "^5.*") {
  QT += widgets
}


CONFIG += plugins
CONFIG += dll


DEFINES += BSAEXTRACTOR_LIBRARY

SOURCES += bsaextractor.cpp

HEADERS += bsaextractor.h

include(../plugin_template.pri)

CONFIG(debug, debug|release) {
  LIBS += -L$$OUT_PWD/../../bsatk/debug
} else {
  LIBS += -L$$OUT_PWD/../../bsatk/release
}


INCLUDEPATH += ../../bsatk "$(BOOSTPATH)"

LIBS += -L"$(BOOSTPATH)/stage/lib" -lbsatk
LIBS += -L"$(ZLIBPATH)/build" -lzlibstatic
OTHER_FILES += \
    bsaextractor.json
