#ifndef EQDIALOG_H
#define EQDIALOG_H

#include <QDialog>
#include <QTimer>
#include "ipc.h"

namespace Ui {
class EqDialog;
}

class EqDialog : public QDialog
{
    Q_OBJECT

private:
    QVector<double> bands;
    double interpolation(double bandStartFreq, double bandStartVal, double bandEndFreq, double bandEndVal, double frequency);

public:
    explicit EqDialog(IPC *pIPC, QWidget *parent = 0);
    ~EqDialog();
    QVector<double> getBands();

private slots:
    void updateEq();

private:
    Ui::EqDialog *ui;
    IPC *ipc;
};

#endif // EQDIALOG_H
