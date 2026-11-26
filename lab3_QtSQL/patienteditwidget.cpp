#include "patienteditwidget.h"

#include <QGridLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QDate>

PatientEditWidget::PatientEditWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void PatientEditWidget::setupUi()
{
    this->setStyleSheet("background-color: #f0f0f0;");

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 30, 40, 30);

    auto *formLayout = new QGridLayout();
    formLayout->setHorizontalSpacing(10);
    formLayout->setVerticalSpacing(12);
    formLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    m_idEdit = new QLineEdit(this);
    m_idEdit->setFixedWidth(180);
    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setFixedWidth(180);
    m_idCardEdit = new QLineEdit(this);
    m_idCardEdit->setFixedWidth(180);

    m_sexCombo = new QComboBox(this);
    m_sexCombo->setFixedWidth(180);
    m_sexCombo->addItem("男", 1);
    m_sexCombo->addItem("女", 0);

    m_dobEdit = new QDateEdit(this);
    m_dobEdit->setFixedWidth(180);
    m_dobEdit->setDisplayFormat("yyyy/M/d");
    m_dobEdit->setDate(QDate(2000, 1, 1));

    m_heightSpin = new QDoubleSpinBox(this);
    m_heightSpin->setFixedWidth(180);
    m_heightSpin->setRange(0, 300);
    m_heightSpin->setSingleStep(0.1);

    m_weightSpin = new QDoubleSpinBox(this);
    m_weightSpin->setFixedWidth(180);
    m_weightSpin->setRange(0, 500);
    m_weightSpin->setSingleStep(0.1);

    m_mobileEdit = new QLineEdit(this);
    m_mobileEdit->setFixedWidth(180);

    m_saveBtn = new QPushButton("保存", this);
    m_saveBtn->setFixedWidth(90);
    m_cancelBtn = new QPushButton("取消", this);
    m_cancelBtn->setFixedWidth(90);

    int row = 0;
    formLayout->addWidget(new QLabel("ID：", this), row, 0, Qt::AlignRight);
    formLayout->addWidget(m_idEdit, row, 1);
    ++row;
    formLayout->addWidget(new QLabel("姓名：", this), row, 0, Qt::AlignRight);
    formLayout->addWidget(m_nameEdit, row, 1);
    ++row;
    formLayout->addWidget(new QLabel("身份证：", this), row, 0, Qt::AlignRight);
    formLayout->addWidget(m_idCardEdit, row, 1);
    ++row;
    formLayout->addWidget(new QLabel("性别：", this), row, 0, Qt::AlignRight);
    formLayout->addWidget(m_sexCombo, row, 1);
    ++row;
    formLayout->addWidget(new QLabel("出生日期：", this), row, 0, Qt::AlignRight);
    formLayout->addWidget(m_dobEdit, row, 1);
    ++row;
    formLayout->addWidget(new QLabel("身高：", this), row, 0, Qt::AlignRight);
    formLayout->addWidget(m_heightSpin, row, 1);
    ++row;
    formLayout->addWidget(new QLabel("体重：", this), row, 0, Qt::AlignRight);
    formLayout->addWidget(m_weightSpin, row, 1);
    formLayout->addWidget(m_saveBtn, row, 2);
    ++row;
    formLayout->addWidget(new QLabel("手机号：", this), row, 0, Qt::AlignRight);
    formLayout->addWidget(m_mobileEdit, row, 1);
    formLayout->addWidget(m_cancelBtn, row, 2);

    mainLayout->addLayout(formLayout);
    mainLayout->addStretch();
}
