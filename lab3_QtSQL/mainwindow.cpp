#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "loginwidget.h"
#include "welcomewidget.h"
#include "patientwidget.h"
#include "patienteditwidget.h"

#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUi();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUi()
{
    this->resize(800, 600);
    this->setWindowTitle("医院诊疗系统");

    auto *central = new QWidget(this);
    this->setCentralWidget(central);

    auto *rootLayout = new QVBoxLayout(central);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // 顶部标题栏：返回 + 标题 + 全屏
    auto *titleBar = new QHBoxLayout();
    titleBar->setContentsMargins(10, 8, 10, 8);
    titleBar->setSpacing(10);

    m_backBtn = new QPushButton("<", this);
    m_backBtn->setFixedWidth(40);
    m_backBtn->setFlat(true);

    m_titleLabel = new QLabel("登录", this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);

    m_fullBtn = new QPushButton(this);
    m_fullBtn->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));
    m_fullBtn->setFixedWidth(40);
    m_fullBtn->setFlat(true);

    titleBar->addWidget(m_backBtn);
    titleBar->addStretch();
    titleBar->addWidget(m_titleLabel);
    titleBar->addStretch();
    titleBar->addWidget(m_fullBtn);

    rootLayout->addLayout(titleBar);

    // 页面堆栈
    m_stack = new QStackedWidget(this);
    m_loginPage = new LoginWidget(this);
    m_welcomePage = new WelcomeWidget(this);
    m_patientPage = new PatientWidget(this);
    m_patientEditPage = new PatientEditWidget(this);

    m_stack->addWidget(m_loginPage);       // 0
    m_stack->addWidget(m_welcomePage);     // 1
    m_stack->addWidget(m_patientPage);     // 2
    m_stack->addWidget(m_patientEditPage);  // 3

    rootLayout->addWidget(m_stack, 1);

    // 状态栏：开发者学号与姓名
    ui->statusbar->showMessage("学号：2024414290206    姓名：程迪宏");
}

void MainWindow::setPageTitle(const QString &title)
{
    m_titleLabel->setText(title);
}
