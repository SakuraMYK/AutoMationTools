QT       += core gui axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat
CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    debuginfo.cpp \
    excelscripter.cpp \
    gbkfileio.cpp \
    loganalysis.cpp \
    main.cpp \
    mainwindow.cpp \
    testsuite.cpp \
    totaltestsuite.cpp \
    windowshwnd.cpp

HEADERS += \
    debuginfo.h \
    excelscripter.h \
    gbkfileio.h \
    loganalysis.h \
    mainwindow.h \
    testsuite.h \
    totaltestsuite.h \
    windowshwnd.h

FORMS += \
    debuginfo.ui \
    excelscripter.ui \
    loganalysis.ui \
    mainwindow.ui \
    testsuite.ui \
    totaltestsuite.ui \
    windowshwnd.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icon.qrc
