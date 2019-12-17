#include "examination_page.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QDate>
#include <QDesktopServices>
#include <QDebug>

ExaminationPage::ExaminationPage(QWidget *parent) : QWidget(parent)
{
    QWidget *page = new QWidget(this);
    QFormLayout *layout = new QFormLayout(page);
    page->setLayout(layout);
    this->setWindowTitle("优麒麟上机操作题");

    QGroupBox *info_box = new QGroupBox(page);
    QHBoxLayout *info_layout = new QHBoxLayout();
    info_layout->setSpacing(100);
    info_box->setLayout(info_layout);
    name_info = new QLabel(page);
    info_layout->addWidget(name_info);
    time_info = new QLabel(page);
    info_layout->addWidget(time_info, Qt::AlignCenter);
    layout->addWidget(info_box);

    m_question_list<<"1. 使用截图工具截取整个桌面图片，并保存到本地图片文件夹。"
                   <<"2. 使用软件商店安装一款应用，比如网易云音乐。"
                   <<"3. 将 WPS 文字 添加到开始菜单旁的快速启动栏。"
                   <<"4. 使用文件管理器的预览功能查看文件。"
                   <<"5. 使用优客助手查看系统信息。"
                   <<"6. 调节系统亮度。"
                   <<"7. 添加自定义系统快捷键。"
                   <<"8. 修改搜狗拼音输入法的快捷键设置。"
                   <<"9. 修改默认的软件仓库源为阿里云：http://mirrors.aliyun.com/ubuntu/"
                   <<"10. 设置一个应用为开机启动项。"
                   <<"11. 完成一局中等难度(16x16)扫雷。"
                   <<"12. 用自定义的图像（比如个人头像等）替换开始菜单的图标。";

    for(int i=0;i<m_question_list.size();i++)
    {
        QLabel *question = new QLabel(page);
        question->setText(m_question_list[i]);
        question->setFixedWidth(this->geometry().width());
        layout->addWidget(question);
        m_lable_list<<question;
    }

    QGroupBox *finish_box = new QGroupBox(page);
    QHBoxLayout *finish_layout = new QHBoxLayout();
    finish_box->setLayout(finish_layout);
    finish_layout->setSpacing(100);

    m_finished_info = new QLabel(page);
    m_finished_info->setText("已完成题目:");
    finish_layout->addWidget(m_finished_info);

    m_score_info = new QLabel(page);
    m_score_info->setText("当前得分:");
    finish_layout->addWidget(m_score_info, Qt::AlignLeft);
    layout->addWidget(finish_box);
    updateScore(0);

    QGroupBox *commit_box = new QGroupBox(page);
    QHBoxLayout *commit_layout = new QHBoxLayout();
    commit_box->setLayout(commit_layout);
    QPushButton *commit_btn = new QPushButton(page);
    commit_btn->setText("交卷");
    commit_btn->setFixedWidth(120);
    commit_layout->addWidget(commit_btn, Qt::AlignLeft);
    layout->addWidget(commit_box);

    connect(commit_btn, &QPushButton::clicked, [=](){
        QMessageBox confirmBox;
        confirmBox.setWindowTitle("温馨提示");
        confirmBox.setText("您当前得分为: " + QString("%1").arg(m_score));
        confirmBox.setInformativeText("确认要现在交卷吗？");
        confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        confirmBox.setDefaultButton(QMessageBox::Yes);
        int ret = confirmBox.exec();

        if (ret == QMessageBox::Yes)
        {
            Q_EMIT this->commitTest();
        }

    });

    startCheckProcess();
    m_cycle_timer = new QTimer(this);
    m_cycle_timer->setInterval(5000);
    m_cycle_timer->start(5000);
    connect(m_cycle_timer, &QTimer::timeout, this, &ExaminationPage::checkProcess);
}

void ExaminationPage::checkProcess()
{
    //qDebug()<<m_screenshot_monitor->state();

    QString userPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    if (m_done_flag[0] == 0)
    {
        QString path= userPath + "/图片";
        QDir *dir=new QDir(path);
        QStringList filter;
        filter<<"*.png";
        dir->setNameFilters(filter);
        QList<QFileInfo> *fileInfo=new QList<QFileInfo>(dir->entryInfoList(filter));
        if (png_count == -1)
            png_count = fileInfo->count();
        else if (fileInfo->count() > png_count){
            updateFineshed(1);
        }
    }

    if (m_done_flag[1] == 0)
    {
        //检查是否有新安装的程序
        QFile file("/var/log/apt/history.log");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray t = file.readAll();
        QString log = QString(t);
        //qDebug()<<"history:"<<log;
        file.close();

        if (app_install_count == -1) //first count
        {
            app_install_count = log.count("Start-Date");
        }
        else if (log.count("Start-Date") > app_install_count){
           updateFineshed(2);
        }
    }

    if (m_done_flag[2] == 0)
    {
        QFile file(userPath + "/.applet");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray t = file.readAll();
        QString list = QString(t);
        //qDebug()<<"history:"<<list;
        file.close();
        if (t.contains("WPS 文字"))
            updateFineshed(3);
    }

    if (m_done_flag[7] == 0)
    {
        QFile file(userPath + "/.config/SogouPY/sogouEnv.ini");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray t = file.readAll();
        QString list = QString(t);
        //qDebug()<<"m_sogou_setting:"<<list;
        file.close();

        if (m_sogou_setting == "")
            m_sogou_setting = list;
        else if (m_sogou_setting != list)
            updateFineshed(8);
    }

    if (m_done_flag[8] == 0)
    {
        QFile file("/etc/apt/sources.list");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray t = file.readAll();
        QString list = QString(t);
        //qDebug()<<"sources.list:"<<list;
        file.close();
        if (t.contains("http://mirrors.aliyun.com/ubuntu/"))
            updateFineshed(9);
    }

    if (m_done_flag[9] == 0)
    {
        QString path= userPath + "/.config/autostart";
        QDir *dir=new QDir(path);
        QStringList filter;
        filter<<"*.desktop";
        dir->setNameFilters(filter);
        QList<QFileInfo> *fileInfo=new QList<QFileInfo>(dir->entryInfoList(filter));

        //qDebug()<<fileInfo->count()<<auto_start_count<<fileInfo->size()<<dir->exists();
        if (auto_start_count == -1)
            auto_start_count = fileInfo->count();
        else if (fileInfo->count() > auto_start_count)
        {
            updateFineshed(10);
        }
    }

    //.local/share/gnome-mines/scores
}

void ExaminationPage::setUserName(QString name)
{
    name_info->setText("考生姓名:" + name);
}

void ExaminationPage::setTimeInfo(QString minStr, QString secStr)
{
    time_info->setText("考试时间剩余: " + minStr + "分钟" + secStr + "秒");
    if (minStr.toInt() < 15)
        time_info->setStyleSheet("color:red;");
}

void ExaminationPage::updateFineshed(int index)
{
    QString cur_text = m_finished_info->text();
    cur_text += QString("%1").arg(index) + ",";
    m_finished_info->setText(cur_text);
    m_finished_info->setStyleSheet("color:green;");
    updateScore(m_question_score[index-1]);
    m_done_flag[index-1] = 1;
    m_lable_list[index-1]->setText(m_question_list[index-1] + " 完成!");
    m_lable_list[index-1]->setStyleSheet("color:green;");

    QMessageBox tipsBox;
    tipsBox.setWindowTitle("完成操作提示");
    tipsBox.setText("您完成了了操作题：" + QString("%1").arg(index));
    tipsBox.setInformativeText("积分增加：" + QString("%1").arg(m_question_score[index-1]));
    tipsBox.setStandardButtons(QMessageBox::Ok);
    tipsBox.exec();
}

void ExaminationPage::updateScore(int addScore)
{
    m_score += addScore;
    QString score_info = "当前得分:" + QString("%1").arg(m_score);
    m_score_info->setText(score_info);
    m_score_info->setStyleSheet("color:red;");
}

void ExaminationPage::startCheckProcess()
{
    qDebug()<<"startCheckProcess";
    m_screenshot_monitor = new QProcess(this);
    m_screenshot_monitor->setProgram("terminate");
    QStringList args{"ps", "-aux", "|", "grep", "gnome-screensho"};
    m_screenshot_monitor->setArguments(args);
    m_screenshot_monitor->startDetached();
}

void ExaminationPage::clearProcess()
{
    m_screenshot_monitor->close();
}
