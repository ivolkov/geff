#ifndef NORM_H
#define NORM_H

#include <QDialog>
#include <QTimer>
#include "ipc.h"

namespace Ui {
class Comp;
}

class Comp : public QDialog
{
    Q_OBJECT

public:
    explicit Comp(IPC *pIPC, QWidget *parent = 0);
    ~Comp();

private:
    Ui::Comp *ui;
    IPC *ipc;
    QTimer tmr;

public slots:

private slots:
    void on_cbEnable_clicked();
    void on_slThreshold_valueChanged(int value);
    void on_slRatio_valueChanged(int value);
    void on_slAttack_valueChanged(int value);
    void on_slRelease_valueChanged(int value);
};

#endif // NORM_H
