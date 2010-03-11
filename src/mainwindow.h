#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "megaprotocol.h"
#include "tablelistwidget.h"
#include "recordlistwidget.h"
#include "megatcpsocket.h"
#include "singleton.h"
class TableListWidget;
class MainWindow : public QMainWindow, public Singleton<MainWindow>
{
    Q_OBJECT
protected:
    MainWindow(QWidget *parent = 0);
    friend class Singleton<MainWindow>;
    void closeEvent(QCloseEvent *event);
public:
    void setStatusLabelText(const QString&);
    void writeSettings();
    void show();
private:
    void readSettings();
    QStatusBar *statusBar;
    QLabel *statusLabel;
    TableListWidget *tableListWidget;
};

#endif // MAINWINDOW_H
