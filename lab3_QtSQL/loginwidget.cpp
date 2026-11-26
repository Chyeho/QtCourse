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
    this->setStyleSheet("background-color: #f0f0f0;");

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 30, 40, 30);
    mainLayout->setSpacing(20);

    // 欢迎标题
    auto *titleLabel = new QLabel("欢迎使用诊疗测试系统", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    mainLayout->addSpacing(30);

    // 表单区域
    auto *formLayout = new QGridLayout();
    formLayout->setHorizontalSpacing(10);
    formLayout->setVerticalSpacing(15);
    formLayout->setAlignment(Qt::AlignCenter);

    auto *userLabel = new QLabel("用户名：", this);
    m_usernameEdit = new QLineEdit(this);
    m_usernameEdit->setFixedWidth(180);
    m_usernameEdit->setText("admin");

    auto *pwdLabel = new QLabel("密码：", this);
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setFixedWidth(180);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setText("123456");

    formLayout->addWidget(userLabel, 0, 0, Qt::AlignRight);
    formLayout->addWidget(m_usernameEdit, 0, 1);
    formLayout->addWidget(pwdLabel, 1, 0, Qt::AlignRight);
    formLayout->addWidget(m_passwordEdit, 1, 1);

    mainLayout->addLayout(formLayout);

    // 按钮行
    auto *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(20);
    btnLayout->setAlignment(Qt::AlignCenter);

    m_registerBtn = new QPushButton("注册", this);
    m_registerBtn->setFixedWidth(100);
    m_loginBtn = new QPushButton("登录", this);
    m_loginBtn->setFixedWidth(100);

    btnLayout->addWidget(m_registerBtn);
    btnLayout->addWidget(m_loginBtn);
    mainLayout->addLayout(btnLayout);

    mainLayout->addStretch();
}
