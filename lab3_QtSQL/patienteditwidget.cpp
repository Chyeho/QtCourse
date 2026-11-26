#include "patienteditwidget.h"

#include <QGridLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QDate>
#include <QRegularExpression>

PatientEditWidget::PatientEditWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void PatientEditWidget::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 30, 40, 30);
    mainLayout->addStretch();

    // 白色卡片面板
    auto *panel = new QWidget(this);
    panel->setObjectName("cardPanel");

    auto *formLayout = new QGridLayout(panel);
    formLayout->setHorizontalSpacing(10);
    formLayout->setVerticalSpacing(12);
    formLayout->setContentsMargins(30, 25, 30, 25);
    formLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    m_idEdit = new QLineEdit(panel);
    m_idEdit->setFixedWidth(180);
    m_nameEdit = new QLineEdit(panel);
    m_nameEdit->setFixedWidth(180);
    m_idCardEdit = new QLineEdit(panel);
    m_idCardEdit->setFixedWidth(180);

    m_sexCombo = new QComboBox(panel);
    m_sexCombo->setFixedWidth(180);
    m_sexCombo->addItem("男", 1);
    m_sexCombo->addItem("女", 0);

    m_dobEdit = new QDateEdit(panel);
    m_dobEdit->setFixedWidth(180);
    m_dobEdit->setDisplayFormat("yyyy/M/d");
    m_dobEdit->setDate(QDate(2000, 1, 1));

    m_heightSpin = new QDoubleSpinBox(panel);
    m_heightSpin->setFixedWidth(180);
    m_heightSpin->setRange(0, 300);
    m_heightSpin->setSingleStep(0.1);

    m_weightSpin = new QDoubleSpinBox(panel);
    m_weightSpin->setFixedWidth(180);
    m_weightSpin->setRange(0, 500);
    m_weightSpin->setSingleStep(0.1);

    m_mobileEdit = new QLineEdit(panel);
    m_mobileEdit->setFixedWidth(180);

    m_saveBtn = new QPushButton("保存", panel);
    m_saveBtn->setFixedWidth(90);
    m_saveBtn->setProperty("primary", true);
    m_cancelBtn = new QPushButton("取消", panel);
    m_cancelBtn->setFixedWidth(90);

    int row = 0;
    formLayout->addWidget(new QLabel("ID：", panel), row, 0, Qt::AlignRight);
    formLayout->addWidget(m_idEdit, row, 1);
    ++row;
    formLayout->addWidget(new QLabel("姓名：", panel), row, 0, Qt::AlignRight);
    formLayout->addWidget(m_nameEdit, row, 1);
    ++row;
    formLayout->addWidget(new QLabel("身份证：", panel), row, 0, Qt::AlignRight);
    formLayout->addWidget(m_idCardEdit, row, 1);
    ++row;
    formLayout->addWidget(new QLabel("性别：", panel), row, 0, Qt::AlignRight);
    formLayout->addWidget(m_sexCombo, row, 1);
    ++row;
    formLayout->addWidget(new QLabel("出生日期：", panel), row, 0, Qt::AlignRight);
    formLayout->addWidget(m_dobEdit, row, 1);
    ++row;
    formLayout->addWidget(new QLabel("身高：", panel), row, 0, Qt::AlignRight);
    formLayout->addWidget(m_heightSpin, row, 1);
    ++row;
    formLayout->addWidget(new QLabel("体重：", panel), row, 0, Qt::AlignRight);
    formLayout->addWidget(m_weightSpin, row, 1);
    formLayout->addWidget(m_saveBtn, row, 2);
    ++row;
    formLayout->addWidget(new QLabel("手机号：", panel), row, 0, Qt::AlignRight);
    formLayout->addWidget(m_mobileEdit, row, 1);
    formLayout->addWidget(m_cancelBtn, row, 2);

    mainLayout->addWidget(panel, 0, Qt::AlignCenter);
    mainLayout->addStretch();
}

void PatientEditWidget::clearForm()
{
    m_isAddMode = true;
    m_idEdit->setReadOnly(false);
    m_idEdit->clear();
    m_nameEdit->clear();
    m_idCardEdit->clear();
    m_sexCombo->setCurrentIndex(0);
    m_dobEdit->setDate(QDate(2000, 1, 1));
    m_heightSpin->setValue(0);
    m_weightSpin->setValue(0);
    m_mobileEdit->clear();
}

void PatientEditWidget::loadRecord(const QSqlRecord &rec)
{
    m_isAddMode = false;
    m_idEdit->setReadOnly(true);
    m_idEdit->setText(rec.value("ID").toString());
    m_nameEdit->setText(rec.value("NAME").toString());
    m_idCardEdit->setText(rec.value("ID_CARD").toString());

    const int sex = rec.value("SEX").toInt();
    m_sexCombo->setCurrentIndex(sex == 1 ? 0 : 1);

    const QDate dob = QDate::fromString(rec.value("DOB").toString(), "yyyy/M/d");
    m_dobEdit->setDate(dob.isValid() ? dob : QDate(2000, 1, 1));

    m_heightSpin->setValue(rec.value("HEIGHT").toDouble());
    m_weightSpin->setValue(rec.value("WEIGHT").toDouble());
    m_mobileEdit->setText(rec.value("MOBILEPHONE").toString());
}

bool PatientEditWidget::validate(QString &error) const
{
    if (m_idEdit->text().trimmed().isEmpty()) {
        error = "患者编号不能为空";
        return false;
    }
    if (m_nameEdit->text().trimmed().isEmpty()) {
        error = "患者姓名不能为空";
        return false;
    }

    const QString idCard = m_idCardEdit->text().trimmed();
    if (!idCard.isEmpty() && idCard.length() != 18) {
        error = "身份证号应为 18 位（留空可跳过）";
        return false;
    }

    const QString mobile = m_mobileEdit->text().trimmed();
    if (!mobile.isEmpty() &&
        !QRegularExpression(QStringLiteral("^1\\d{10}$")).match(mobile).hasMatch()) {
        error = "手机号应为 11 位数字（留空可跳过）";
        return false;
    }
    return true;
}
