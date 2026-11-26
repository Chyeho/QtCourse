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
#include <QApplication>
#include <QMessageBox>

namespace {
// 全局样式表：医疗蓝主题，简洁扁平
const char *g_styleSheet = R"(
QWidget {
    background-color: #eef3f8;
    color: #22303c;
    font-size: 13px;
}

/* 顶部标题栏 */
#titleBar {
    background-color: #2b7cbe;
}
#titleBar QLabel {
    background-color: transparent;
    color: #ffffff;
    font-size: 14px;
    font-weight: bold;
}
#titleBar QPushButton {
    background-color: transparent;
    color: #ffffff;
    border: none;
    border-radius: 4px;
    padding: 4px;
}
#titleBar QPushButton:hover {
    background-color: rgba(255, 255, 255, 0.18);
}

/* 输入控件 */
QLineEdit, QComboBox, QDateEdit, QDoubleSpinBox, QSpinBox {
    background-color: #ffffff;
    border: 1px solid #c9dcec;
    border-radius: 4px;
    padding: 5px 8px;
    min-height: 20px;
}
QLineEdit:focus, QComboBox:focus, QDateEdit:focus,
QDoubleSpinBox:focus, QSpinBox:focus {
    border: 1px solid #2b7cbe;
}

/* 普通按钮：白底蓝边蓝字 */
QPushButton {
    background-color: #ffffff;
    color: #2b7cbe;
    border: 1px solid #2b7cbe;
    border-radius: 4px;
    padding: 6px 16px;
}
QPushButton:hover { background-color: #e8f2fb; }
QPushButton:pressed { background-color: #d6e8f6; }

/* 主按钮：蓝底白字 */
QPushButton[primary="true"] {
    background-color: #2b7cbe;
    color: #ffffff;
    border: 1px solid #2b7cbe;
}
QPushButton[primary="true"]:hover { background-color: #1f669e; }
QPushButton[primary="true"]:pressed { background-color: #1a5a8c; }

/* 欢迎页入口卡片 */
QPushButton[card="true"] {
    background-color: #ffffff;
    color: #22303c;
    border: 1px solid #c9dcec;
    border-radius: 10px;
    font-size: 15px;
}
QPushButton[card="true"]:hover {
    background-color: #e8f2fb;
    border: 1px solid #2b7cbe;
    color: #2b7cbe;
}

/* 白色内容面板（登录 / 编辑表单容器） */
#cardPanel {
    background-color: #ffffff;
    border: 1px solid #dce8f2;
    border-radius: 10px;
}

/* 表格 */
QTableView {
    background-color: #ffffff;
    border: 1px solid #c9dcec;
    border-radius: 4px;
    gridline-color: #e4edf4;
    selection-background-color: #d6e8f6;
    selection-color: #22303c;
}
QHeaderView::section {
    background-color: #2b7cbe;
    color: #ffffff;
    border: none;
    padding: 6px;
    font-weight: bold;
}

/* 状态栏 */
QStatusBar {
    background-color: #e2ecf4;
    color: #4a6274;
}
)";
}

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
    qApp->setStyleSheet(QString::fromUtf8(g_styleSheet));

    auto *central = new QWidget(this);
    this->setCentralWidget(central);

    auto *rootLayout = new QVBoxLayout(central);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // 顶部标题栏：返回 + 标题 + 全屏
    auto *titleBarWidget = new QWidget(this);
    titleBarWidget->setObjectName("titleBar");
    auto *titleBar = new QHBoxLayout(titleBarWidget);
    titleBar->setContentsMargins(10, 8, 10, 8);
    titleBar->setSpacing(10);

    m_backBtn = new QPushButton("<", titleBarWidget);
    m_backBtn->setFixedWidth(40);

    m_titleLabel = new QLabel("登录", titleBarWidget);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);

    m_fullBtn = new QPushButton(titleBarWidget);
    m_fullBtn->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));
    m_fullBtn->setFixedWidth(40);

    titleBar->addWidget(m_backBtn);
    titleBar->addStretch();
    titleBar->addWidget(m_titleLabel);
    titleBar->addStretch();
    titleBar->addWidget(m_fullBtn);

    rootLayout->addWidget(titleBarWidget);

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

    connectSignals();
}

void MainWindow::setPageTitle(const QString &title)
{
    m_titleLabel->setText(title);
}

void MainWindow::gotoPage(PageIndex index, const QString &title)
{
    m_stack->setCurrentIndex(index);
    setPageTitle(title);
}

void MainWindow::goBack()
{
    switch (m_stack->currentIndex()) {
    case PagePatientEdit:
        gotoPage(PagePatient, "患者管理");
        break;
    case PagePatient:
        gotoPage(PageWelcome, "欢迎");
        break;
    case PageWelcome:
        gotoPage(PageLogin, "登录");
        break;
    default:
        break;
    }
}

void MainWindow::connectSignals()
{
    // 登录 -> 欢迎页（凭据校验在后续步骤接入）
    connect(m_loginPage->loginButton(), &QPushButton::clicked, this,
            [this] { gotoPage(PageWelcome, "欢迎"); });

    // 欢迎页入口
    connect(m_welcomePage->patientButton(), &QPushButton::clicked, this,
            [this] { gotoPage(PagePatient, "患者管理"); });
    connect(m_welcomePage->departmentButton(), &QPushButton::clicked, this,
            [this] { QMessageBox::information(this, "提示", "科室管理为选做功能，暂未实现"); });
    connect(m_welcomePage->doctorButton(), &QPushButton::clicked, this,
            [this] { QMessageBox::information(this, "提示", "医生管理为选做功能，暂未实现"); });

    // 患者管理 -> 编辑页（新增 / 修改共用表单，具体数据在后续步骤接入）
    connect(m_patientPage->addButton(), &QPushButton::clicked, this,
            [this] { gotoPage(PagePatientEdit, "编辑患者信息"); });
    connect(m_patientPage->editButton(), &QPushButton::clicked, this,
            [this] { gotoPage(PagePatientEdit, "编辑患者信息"); });

    // 编辑页取消 -> 返回患者管理
    connect(m_patientEditPage->cancelButton(), &QPushButton::clicked, this,
            [this] { gotoPage(PagePatient, "患者管理"); });

    // 标题栏返回
    connect(m_backBtn, &QPushButton::clicked, this, &MainWindow::goBack);

    // 标题栏全屏 / 还原
    connect(m_fullBtn, &QPushButton::clicked, this, [this] {
        if (isFullScreen())
            showNormal();
        else
            showFullScreen();
    });
}
