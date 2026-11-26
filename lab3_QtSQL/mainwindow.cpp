#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "loginwidget.h"
#include "welcomewidget.h"
#include "patientwidget.h"
#include "patientmodel.h"
#include "patienteditwidget.h"
#include "database.h"

#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStyle>
#include <QApplication>
#include <QMessageBox>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QTableView>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDate>
#include <QDateTime>
#include <QRegularExpression>

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

    // 建立数据库连接，失败给出明确提示
    if (!Database::instance().open()) {
        QMessageBox::critical(this, "数据库连接失败",
                              "无法连接到 MySQL 数据库：\n" + Database::instance().lastError());
    }

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

void MainWindow::doLogin()
{
    const QString username = m_loginPage->usernameEdit()->text().trimmed();
    const QString password = m_loginPage->passwordEdit()->text();

    // 必要字段检查
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入用户名和密码");
        return;
    }

    if (!Database::instance().isOpen()) {
        QMessageBox::critical(this, "提示", "数据库未连接，无法登录");
        return;
    }

    QSqlQuery query(Database::instance().db());
    query.prepare("SELECT ID, FULLNAME FROM `user` WHERE USERNAME = ? AND PASSWORD = ?");
    query.addBindValue(username);
    query.addBindValue(password);

    if (!query.exec()) {
        QMessageBox::critical(this, "登录失败", "查询出错：\n" + query.lastError().text());
        return;
    }

    if (query.next()) {
        m_currentUserId = query.value("ID").toString();
        m_currentUserName = query.value("FULLNAME").toString();
        gotoPage(PageWelcome, "欢迎");
    } else {
        QMessageBox::warning(this, "登录失败", "用户名或密码错误");
    }
}

void MainWindow::doAddPatient()
{
    m_patientEditPage->clearForm();
    m_patientEditPage->idEdit()->setText(nextPatientId());
    gotoPage(PagePatientEdit, "新增患者信息");
}

void MainWindow::doEditPatient()
{
    auto *model = m_patientPage->model();
    const int row = m_patientPage->tableView()->currentIndex().row();
    if (!model || row < 0) {
        QMessageBox::warning(this, "提示", "请先在列表中选择要修改的患者记录");
        return;
    }

    const QSqlRecord rec = model->record(row);
    m_patientEditPage->loadRecord(rec);
    gotoPage(PagePatientEdit, "修改患者信息");
}

void MainWindow::doSavePatient()
{
    QString error;
    if (!m_patientEditPage->validate(error)) {
        QMessageBox::warning(this, "输入有误", error);
        return;
    }

    auto *edit = m_patientEditPage;
    const QString id = edit->idEdit()->text().trimmed();
    const QString name = edit->nameEdit()->text().trimmed();
    const QString idCard = edit->idCardEdit()->text().trimmed();
    const int sex = edit->sexCombo()->currentData().toInt();
    const QDate dob = edit->dobEdit()->date();
    const double height = edit->heightSpin()->value();
    const double weight = edit->weightSpin()->value();
    const QString mobile = edit->mobileEdit()->text().trimmed();
    const int age = calcAge(dob);
    const QString dobStr = dob.toString("yyyy/M/d");

    QSqlQuery query(Database::instance().db());
    bool ok = false;

    if (edit->isAddMode()) {
        query.prepare(
            "INSERT INTO patient "
            "(ID, ID_CARD, NAME, SEX, DOB, HEIGHT, WEIGHT, MOBILEPHONE, AGE, CREATEDTIMESTAMP) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        query.addBindValue(id);
        query.addBindValue(idCard);
        query.addBindValue(name);
        query.addBindValue(sex);
        query.addBindValue(dobStr);
        query.addBindValue(height);
        query.addBindValue(weight);
        query.addBindValue(mobile);
        query.addBindValue(age);
        query.addBindValue(QDateTime::currentDateTime().toString("yyyy/M/d hh:mm:ss"));
        ok = query.exec();
    } else {
        query.prepare(
            "UPDATE patient SET ID_CARD = ?, NAME = ?, SEX = ?, DOB = ?, "
            "HEIGHT = ?, WEIGHT = ?, MOBILEPHONE = ?, AGE = ? WHERE ID = ?");
        query.addBindValue(idCard);
        query.addBindValue(name);
        query.addBindValue(sex);
        query.addBindValue(dobStr);
        query.addBindValue(height);
        query.addBindValue(weight);
        query.addBindValue(mobile);
        query.addBindValue(age);
        query.addBindValue(id);
        ok = query.exec();
    }

    if (!ok) {
        QMessageBox::critical(this, "保存失败", query.lastError().text());
        return;
    }

    QMessageBox::information(this, "提示", "患者信息保存成功");
    m_patientPage->refresh();
    gotoPage(PagePatient, "患者管理");
}

QString MainWindow::nextPatientId() const
{
    QString next = QStringLiteral("P0001");
    QSqlQuery query(Database::instance().db());
    if (query.exec("SELECT MAX(ID) FROM patient") && query.next()
        && !query.value(0).toString().isEmpty()) {
        const int n = query.value(0).toString().mid(1).toInt() + 1;
        next = QString("P%1").arg(n, 4, 10, QChar('0'));
    }
    return next;
}

int MainWindow::calcAge(const QDate &dob)
{
    if (!dob.isValid())
        return 0;
    const QDate now = QDate::currentDate();
    int age = now.year() - dob.year();
    if (now.month() < dob.month()
        || (now.month() == dob.month() && now.day() < dob.day()))
        --age;
    return qMax(age, 0);
}

void MainWindow::connectSignals()
{
    // 登录 -> 校验凭据后进入欢迎页
    connect(m_loginPage->loginButton(), &QPushButton::clicked, this, &MainWindow::doLogin);

    // 欢迎页入口
    connect(m_welcomePage->patientButton(), &QPushButton::clicked, this,
            [this] { gotoPage(PagePatient, "患者管理"); });
    connect(m_welcomePage->departmentButton(), &QPushButton::clicked, this,
            [this] { QMessageBox::information(this, "提示", "科室管理为选做功能，暂未实现"); });
    connect(m_welcomePage->doctorButton(), &QPushButton::clicked, this,
            [this] { QMessageBox::information(this, "提示", "医生管理为选做功能，暂未实现"); });

    // 患者管理 -> 编辑页（新增 / 修改共用表单）
    connect(m_patientPage->addButton(), &QPushButton::clicked, this, &MainWindow::doAddPatient);
    connect(m_patientPage->editButton(), &QPushButton::clicked, this, &MainWindow::doEditPatient);
    connect(m_patientEditPage->saveButton(), &QPushButton::clicked, this, &MainWindow::doSavePatient);

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
