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
    this->setStyleSheet("background-color: #f0f0f0;");

    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(60, 80, 60, 80);
    mainLayout->setSpacing(60);
    mainLayout->setAlignment(Qt::AlignCenter);

    const QString cardStyle =
        "QPushButton {"
        "  background-color: #e8e8e8;"
        "  border: 1px solid #b0b0b0;"
        "  font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #dddddd;"
        "}";

    m_deptBtn = new QPushButton("科室管理", this);
    m_doctorBtn = new QPushButton("医生管理", this);
    m_patientBtn = new QPushButton("患者管理", this);

    for (QPushButton *btn : {m_deptBtn, m_doctorBtn, m_patientBtn}) {
        btn->setFixedSize(120, 120);
        btn->setStyleSheet(cardStyle);
        mainLayout->addWidget(btn);
    }
}
