#include "welcomewidget.h"

#include <QHBoxLayout>
#include <QPushButton>

WelcomeWidget::WelcomeWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void WelcomeWidget::setupUi()
{
    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(60, 80, 60, 80);
    mainLayout->setSpacing(60);
    mainLayout->setAlignment(Qt::AlignCenter);

    m_deptBtn = new QPushButton("科室管理", this);
    m_doctorBtn = new QPushButton("医生管理", this);
    m_patientBtn = new QPushButton("患者管理", this);

    for (QPushButton *btn : {m_deptBtn, m_doctorBtn, m_patientBtn}) {
        btn->setFixedSize(130, 130);
        btn->setProperty("card", true);
        mainLayout->addWidget(btn);
    }
}
