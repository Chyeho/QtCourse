#include "loginwidget.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void LoginWidget::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(60, 40, 60, 40);
    mainLayout->addStretch();

    // 白色卡片面板
    auto *panel = new QWidget(this);
    panel->setObjectName("cardPanel");
    panel->setFixedWidth(360);

    auto *panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(30, 30, 30, 30);
    panelLayout->setSpacing(20);

    // 欢迎标题
    auto *titleLabel = new QLabel("欢迎使用诊疗测试系统", panel);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(15);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #2b7cbe; background: transparent;");
    panelLayout->addWidget(titleLabel);

    // 表单区域
    auto *formLayout = new QGridLayout();
    formLayout->setHorizontalSpacing(10);
    formLayout->setVerticalSpacing(15);

    auto *userLabel = new QLabel("用户名：", panel);
    m_usernameEdit = new QLineEdit(panel);
    m_usernameEdit->setText("admin");

    auto *pwdLabel = new QLabel("密码：", panel);
    m_passwordEdit = new QLineEdit(panel);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setText("123456");

    formLayout->addWidget(userLabel, 0, 0, Qt::AlignRight);
    formLayout->addWidget(m_usernameEdit, 0, 1);
    formLayout->addWidget(pwdLabel, 1, 0, Qt::AlignRight);
    formLayout->addWidget(m_passwordEdit, 1, 1);

    panelLayout->addLayout(formLayout);

    // 按钮行
    auto *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(20);
    btnLayout->setAlignment(Qt::AlignCenter);

    m_registerBtn = new QPushButton("注册", panel);
    m_registerBtn->setFixedWidth(110);
    m_loginBtn = new QPushButton("登录", panel);
    m_loginBtn->setFixedWidth(110);
    m_loginBtn->setDefault(true);
    m_loginBtn->setProperty("primary", true);

    btnLayout->addWidget(m_registerBtn);
    btnLayout->addWidget(m_loginBtn);
    panelLayout->addLayout(btnLayout);

    mainLayout->addWidget(panel, 0, Qt::AlignCenter);
    mainLayout->addStretch();
}
