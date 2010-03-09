# -------------------------------------------------
# Project created by QtCreator 2010-02-18T10:08:33
# -------------------------------------------------
TARGET = MegaClient
TEMPLATE = app
QT += network
SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/tablelistwidget.cpp \
    src/megatcpsocket.cpp \
    src/recordlistwidget.cpp \
    src/tableadedwidget.cpp \
    src/tabledeletewidget.cpp \
    src/recordadedview.cpp
HEADERS += src/mainwindow.h \
    src/megaprotocol.h \
    src/tablelistwidget.h \
    src/megatcpsocket.h \
    src/recordlistwidget.h \
    src/singleton.h \
    src/tableadedwidget.h \
    src/tabledeletewidget.h \
    src/recordadedview.h
FORMS += 
RESOURCES += MegaClient.qrc
TRANSLATIONS += MegaClient_ru.ts
