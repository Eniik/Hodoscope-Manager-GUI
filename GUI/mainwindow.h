#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "communication/serialcommunication.h"
#include "GUI/qcustomplot.h"
#include "kernel/datamanager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void frequencyChanged(float f);
    void azimuthChanged(float a);
    void polarChanged(float p);
    void distanceChanged(float d);

private slots:
    void on_pushButtonStart_clicked();

private:
    Ui::MainWindow *ui;
    DataManager *dataManager;
    bool plotEnabled;

    void plotInit();
};

#endif // MAINWINDOW_H
