QT += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FinTP-Configuration-Wizard
TEMPLATE = app

SOURCES += main.cpp\
        fintpconfigwizard.cpp

HEADERS  += fintpconfigwizard.h

FORMS    +=

#wince50standard-x86-msvc2005: LIBS += libcmt.lib corelibc.lib ole32.lib oleaut32.lib uuid.lib commctrl.lib coredll.lib winsock.lib ws2.lib
