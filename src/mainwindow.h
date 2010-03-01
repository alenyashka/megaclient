#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "megaprotocol.h"
#include "tablelistwidget.h"
#include "recordlistwidget.h"
#include "megatcpsocket.h"
class TableListWidget;
class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:
    static MainWindow *_self;
    static int _refcount;
    MainWindow(QWidget *parent = 0);
    virtual ~MainWindow()
    {
        _self = NULL;
    }
public:
    static MainWindow* Instance(QWidget *parent = 0)
    {
        if (!_self) _self = new MainWindow(parent);
        return _self;
    }
    void FreeInst()
    {
        if (--_refcount == 0)
        {
            delete this;
        }
    }
    void setStatusLabelText(const QString&);
    void show();
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
