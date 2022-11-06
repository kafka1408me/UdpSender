TEMPLATE = app
CONFIG += c++14 precompile_header
CONFIG -= app_bundle
#CONFIG -= qt

TARGET = UdpSender

SOURCES += \
        main.cpp \
        myapp.cpp \
        settings.cpp \
        textevent.cpp \
        window.cpp \
        udpcommunicator.cpp

INCLUDEPATH += D:/CppLibraries/boost_1_77_0/boost_1_77_0

win32{
    LIBS += -lWs2_32
    RC_FILE = myapp.rc
}

#DEFINES += TEST

PRECOMPILED_HEADER += stable.h

QT += core gui widgets

HEADERS += \
    message_type.h \
    myapp.h \
    settings.h \
    textevent.h \
    window.h \
    udpcommunicator.h \
    stable.h

CONFIG(debug, debug|release){
  DESTDIR = $${PWD}\\udp_debug
}else{
  DESTDIR = $${PWD}\\udp_release

    DESTDIR_WIN = $${DESTDIR}
    DESTDIR_WIN ~= s,/,\\,g

    FILES += "$$(QTDIR)\\bin\\libstdc++-6.dll"
    FILES += "$$(QTDIR)\\bin\\Qt5Gui.dll"
    FILES += "$$(QTDIR)\\bin\\Qt5Widgets.dll"
    FILES += "$$(QTDIR)\\bin\\Qt5Core.dll"
    FILES += "$$(QTDIR)\\bin\\libwinpthread-1.dll"
    FILES += $$(QTDIR)\\bin\\libgcc_s_dw2-1.dll
    for(file, FILES) {
        QMAKE_PRE_LINK += $$QMAKE_COPY_FILE \"$$file\" \"$${DESTDIR_WIN}\\$$basename(file)\" $$escape_expand(\n\t)
    }
    exists($${DESTDIR_WIN}\\platforms) {
    }
    else {
    QMAKE_PRE_LINK += $(MKDIR) $${DESTDIR_WIN}\\platforms $$escape_expand(\n\t)
    }
    QMAKE_PRE_LINK += $$QMAKE_COPY_FILE \"$$(QTDIR)\\plugins\\platforms\\qwindows.dll\" \"$${DESTDIR_WIN}\\platforms\\qwindows.dll\" $$escape_expand(\n\t)
}


FORMS += \
    window.ui
