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
    void on_slThreshold_sliderMoved(int position);
    void on_slRatio_sliderMoved(int position);
    void on_slAttack_sliderMoved(int position);
    void on_slRelease_sliderMoved(int position);
};

#endif // NORM_H
