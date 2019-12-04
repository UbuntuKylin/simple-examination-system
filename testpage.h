#ifndef TESTPAGE_H
#define TESTPAGE_H

#include <QMainWindow>

namespace Ui {
class TestPage;
}

class TestPage : public QMainWindow
{
    Q_OBJECT

public:
    explicit TestPage(QWidget *parent = nullptr);
    ~TestPage();

private:
    Ui::TestPage *ui;
};

#endif // TESTPAGE_H
