#ifndef EXAMINATION_PAGE_H
#define EXAMINATION_PAGE_H

#include <QWidget>
#include <QLabel>
#include <QLCDNumber>
#include <QProcess>
#include <QTimer>

class ExaminationPage : public QWidget
{
    Q_OBJECT
public:
    explicit ExaminationPage(QWidget *parent = nullptr);

    void setUserName(QString name);
    void setTimeInfo(QString minStr, QString secStr);
    void updateFineshed(int index);
    void updateScore(int score);

    void startCheckProcess();
    void clearProcess();

Q_SIGNALS:
    void commitTest();

public Q_SLOTS:
    void checkProcess();

public:
    QLabel *name_info;
    QLabel *time_info;
    QLabel *m_finished_info;
    QLabel *m_score_info;

    QStringList m_question_list;
    QString m_sogou_setting = "";

    QProcess *m_screenshot_monitor;
    QTimer *m_cycle_timer;

    int m_score = 0;
    bool first_check = true;
    int app_install_count = -1;
    int auto_start_count = -1;
    int png_count = -1;
    QList<int> m_question_score{10,10,10,10,10,10,10,10,10,10,20,30};
    QList<int> m_done_flag{0,0,0,0,0, 0,0,0,0,0,0,0};

    QList<QLabel*> m_lable_list;


signals:

public slots:
};

#endif // EXAMINATION_PAGE_H
