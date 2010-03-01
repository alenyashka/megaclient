#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

#include "megaprotocol.h"
#include "tablelistwidget.h"
#include "megatcpsocket.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

private slots:
    void setStatusLabelText(const QString &);

private:
    void readSettings();
    void writeSettings();
    QStatusBar *statusBar;
    QLabel *statusLabel;
    TableListWidget *tableListWidget;
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
