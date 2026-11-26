#ifndef WELCOMEWIDGET_H
#define WELCOMEWIDGET_H

#include <QWidget>

class QPushButton;

// 欢迎主页（对应实验参考图 2）：科室管理 / 医生管理 / 患者管理 三个入口卡片
class WelcomeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WelcomeWidget(QWidget *parent = nullptr);

    QPushButton *departmentButton() const { return m_deptBtn; }
    QPushButton *doctorButton() const { return m_doctorBtn; }
    QPushButton *patientButton() const { return m_patientBtn; }

private:
    void setupUi();

    QPushButton *m_deptBtn = nullptr;
    QPushButton *m_doctorBtn = nullptr;
    QPushButton *m_patientBtn = nullptr;
};

#endif // WELCOMEWIDGET_H
