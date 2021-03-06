# -------------------------------------------------
# Project created by QtCreator 2010-02-18T10:08:33
# -------------------------------------------------
TARGET = megaclient
TEMPLATE = app
QT += network
SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/tablelistwidget.cpp \
    src/recordlistwidget.cpp \
    src/tableadedwidget.cpp \
    src/tabledeletewidget.cpp \
    src/recordadedview.cpp \
    src/util.cpp \
    src/recorddeletewidget.cpp \
    src/settingswidget.cpp \
    src/apppath.cpp \
    src/megaguard.cpp \
    src/megaguardwidget.cpp \
    src/megaconnector.cpp
HEADERS += src/mainwindow.h \
    src/megaprotocol.h \
    src/tablelistwidget.h \
    src/recordlistwidget.h \
    src/singleton.h \
    src/tableadedwidget.h \
    src/tabledeletewidget.h \
    src/recordadedview.h \
    src/util.h \
    src/recorddeletewidget.h \
    src/settingswidget.h \
    src/apppath.h \
    src/megaguard.h \
    src/megaguardwidget.h \
    src/megaconnector.h
FORMS += 
RESOURCES += megaclient.qrc
TRANSLATIONS += l10n/megaclient_ru.ts
unix { 
    # VARIABLES
    isEmpty(PREFIX):PREFIX = /usr/local
    BINDIR = $$PREFIX/bin
    DATADIR = $$PREFIX/share
    DEFINES += DATADIR=\"$$DATADIR\" \
        PKGDATADIR=\"$$PKGDATADIR\"
    
    # MAKE INSTALL
    INSTALLS += target \
        desktop \
        pixmaps \
        icon22 \
        l10n
    target.path = $$BINDIR
    desktop.path = $$DATADIR/applications/
    desktop.files += $${TARGET}.desktop
    pixmaps.path = $$DATADIR/pixmaps/
    pixmaps.files += images/$${TARGET}.png
    icon22.path = $$DATADIR/icons/hicolor/22x22/apps/
    icon22.files += images/*.png
    l10n.path = $$DATADIR/l10n/
    l10n.files += l10n/*.qm
}
