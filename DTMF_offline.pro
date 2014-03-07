TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += main.c


unix:!macx:!symbian: LIBS += -L$$PWD/ -ldtmf -lcomm

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/

unix:!macx:!symbian: PRE_TARGETDEPS += $$PWD/libcomm.a $$PWD/libdtmf.a
