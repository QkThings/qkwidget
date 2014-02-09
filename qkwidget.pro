#-------------------------------------------------
#
# Project created by QtCreator 2014-01-04T23:40:03
#
#-------------------------------------------------

QT       += widgets
QT       -= gui

greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport

TARGET = qkwidget
TEMPLATE = lib

DEFINES += QT_NO_DEBUG_OUTPUT

DEFINES += QKWIDGET_LIBRARY

INCLUDEPATH += private

INCLUDEPATH += ../qkcore
LIBS += -lqkcore
QMAKE_LIBDIR += ../qkcore/release

INCLUDEPATH += ../qkconnect
LIBS += -lqkconnect
QMAKE_LIBDIR += ../qkconnect/release

SOURCES += qkexplorerwidget.cpp \
    private/rtplot.cpp \
    private/qhexspinbox.cpp \
    private/qcustomplot.cpp \
    private/ptreewidgetitem.cpp \
    private/ptreeitemproperty.cpp \
    private/ptablewidget.cpp \
    private/loggersettingswidget.cpp \
    private/eventwidget.cpp \
    private/debugsettingswidget.cpp \
    private/cpropertybrowser.cpp \
    private/cproperty.cpp \
    private/boardpanel.cpp \
    private/plotsettings.cpp \
    private/ptoolbutton.cpp

HEADERS += qkexplorerwidget.h\
        qkwidget_global.h \
    private/rtplot.h \
    private/qhexspinbox.h \
    private/qcustomplot.h \
    private/ptreewidgetitem.h \
    private/ptreeitemproperty.h \
    private/ptablewidget.h \
    private/loggersettingswidget.h \
    private/eventwidget.h \
    private/debugsettingswidget.h \
    private/cpropertybrowser.h \
    private/cproperty.h \
    private/boardpanel.h \
    private/plotsettings.h \
    private/gui_globals.h \
    private/ptoolbutton.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

FORMS += \
    qkexplorerwidget.ui \
    private/loggersettingswidget.ui \
    private/eventwidget.ui \
    private/debugsettingswidget.ui \
    private/boardpanel.ui \
    private/plotsettings.ui

RESOURCES += \
    resources/fonts/qkwidget_fonts.qrc \
    resources/icons/qkwidget_icons.qrc \
    resources/img/qkwidget_img.qrc


CONFIG(debug, debug|release) {
    DESTDIR = debug
} else {
    DESTDIR = release
}

OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui
