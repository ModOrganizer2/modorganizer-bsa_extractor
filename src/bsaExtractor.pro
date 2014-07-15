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



CONFIG(debug, debug|release) {
	OUTDIR = $$OUT_PWD/debug
	DSTDIR = $$PWD/../../outputd
	LIBS += -L$$OUT_PWD/../../bsatk/debug
} else {
	OUTDIR = $$OUT_PWD/release
	DSTDIR = $$PWD/../../output
	LIBS += -L$$OUT_PWD/../../bsatk/release
}


DEFINES += BSAEXTRACTOR_LIBRARY

SOURCES += bsaextractor.cpp

HEADERS += bsaextractor.h

include(../plugin_template.pri)

INCLUDEPATH += ../../bsatk "$(BOOSTPATH)"

LIBS += -L"$(BOOSTPATH)/stage/lib" -lbsatk
LIBS += -L"$(ZLIBPATH)/build" -lzlibstatic
OTHER_FILES += \
    bsaextractor.json
