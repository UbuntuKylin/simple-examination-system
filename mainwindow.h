#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "examination_page.h"
#include <QMainWindow>
#include <QStackedWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public Q_SLOTS:
    void startTest();
    void endTest();

    void updateTimer();

private:
    Ui::MainWindow *ui;
    QWidget *m_start_page;
    ExaminationPage *m_exam_page;
    QStackedWidget *m_stacked_widget;

    int m_test_out_time = 3600;  //1 hour for the whole test
    QTimer *timer;

};

#endif // MAINWINDOW_H
