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

INCLUDEPATH += ../../bsatk "$(BOOSTPATH)"

CONFIG(debug, debug|release) {
    LIBS += -L"$(BOOSTPATH)/stage/lib" -L$$OUT_PWD/../../bsatk/debug -lbsatk
} else {
    LIBS += -L"$(BOOSTPATH)/stage/lib" -L$$OUT_PWD/../../bsatk/release -lbsatk
}

LIBS += -L"$(ZLIBPATH)/build" -lzlibstatic
OTHER_FILES += \
    bsaextractor.json
