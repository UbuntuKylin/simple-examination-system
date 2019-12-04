#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "examination_page.h"

#include <QPushButton>
#include <QMessageBox>
#include <QTimer>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_start_page = new QWidget(this);
    m_start_page->setGeometry(this->geometry());
    QFormLayout *layout = new QFormLayout(m_start_page);
    m_start_page->setLayout(layout);
    layout->setContentsMargins(10,50,10,10);
    layout->setAlignment(Qt::AlignCenter);

    layout->addWidget(ui->tips_label);

    QGroupBox *name_info = new QGroupBox(m_start_page);
    QHBoxLayout *name_layout = new QHBoxLayout;
    name_layout->addWidget(ui->name_label);
    name_layout->addWidget(ui->nameEdit, Qt::AlignLeft);
    name_info->setLayout(name_layout);
    layout->addWidget(name_info);

    QGroupBox *time_box = new QGroupBox(m_start_page);
    QHBoxLayout *time_layout = new QHBoxLayout;
    ui->timer_minute->setFixedWidth(60);
    ui->split_label->setFixedWidth(20);
    ui->timer_seconds->setFixedWidth(60);
    time_layout->addWidget(ui->timer_minute);
    time_layout->addWidget(ui->split_label);
    time_layout->addWidget(ui->timer_seconds);
    time_box->setLayout(time_layout);
    layout->addWidget(time_box);
    layout->addWidget(ui->Start_Btn);

    this->setWindowTitle("简单操作展示系统");
    this->setGeometry(500,200,600,400);
    connect(ui->Start_Btn, &QPushButton::clicked, this, &MainWindow::startTest);

    ui->timer_minute->display(m_test_out_time/60);
    ui->timer_seconds->display("00");
}

MainWindow::~MainWindow()
{
    delete ui;
    timer->destroyed();
}

void MainWindow::updateTimer()
{
    //qDebug()<<"updateTimer"<<m_test_out_time;
    m_test_out_time--;
    int min = m_test_out_time/60;
    int sec = m_test_out_time%60;
    QString minStr, secStr;
    minStr = QString("%1").arg(min);
    secStr = QString("%1").arg(sec);
    if (min < 10)
        minStr = "0" + minStr;
    if (sec < 10)
        secStr = "0" + secStr;
    ui->timer_minute->display(minStr);
    ui->timer_seconds->display(secStr);

    m_exam_page->setTimeInfo(minStr, secStr);


    //last 15 minutes, set text to red
    if (m_test_out_time < 15 * 60)
    {
       //todo
    }

    if (m_test_out_time <= 0)
    {
        endTest();
    }
}

void MainWindow::startTest()
{
    if (ui->nameEdit->text() == "")
    {
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setWindowTitle(tr("温馨提示"));
        msgBox->setText(tr("请先输入您的名字再开始考试！"));
        msgBox->exec();
        return;
    }
    qDebug()<<"startTest";
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTimer);
    timer->start(1000);
    ui->Start_Btn->setDisabled(true);

    m_exam_page= new ExaminationPage();
    m_exam_page->setGeometry(this->geometry());
    m_exam_page->setUserName(ui->nameEdit->text());
    m_exam_page->setTimeInfo(QString("%1").arg(m_test_out_time/60), QString("%1").arg(m_test_out_time%60));
    m_exam_page->setGeometry(1200,0,600,600);
    m_exam_page->showNormal();

    connect(m_exam_page, &ExaminationPage::commitTest, this, &MainWindow::endTest);

    this->close();
}

void MainWindow::endTest()
{
    timer->stop();
    m_exam_page->close();
    this->close();
}
