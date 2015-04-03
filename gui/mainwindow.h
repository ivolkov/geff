#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "qcustomplot/qcustomplot.h"
#include "plots.h"
#include "ipc.h"
#include "eq.h"
#include "comp.h"

#define PLOT_CNT 4

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionShow_real_data_triggered();

    void on_actionEqualizer_triggered();

    void on_actionNormalization_triggered();

private:
    Ui::MainWindow *ui;
    QCPItemText *lblFFT;    
    IPCPlots *plots;
    EqDialog *eq;
    IPC ipc;
    Comp *comp;

    bool enableRealData;
};

#endif // MAINWINDOW_H
