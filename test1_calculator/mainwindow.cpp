#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentInput("0")
    , waitForOperand(false)
    , hasError(false)
    , unaryResult(0.0)
{
    ui->setupUi(this);
    setupConnections();
    updateDisplay(currentInput);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupConnections()
{
    // 数字按钮 0-9
    const QMap<QString, QPushButton*> digitButtons = {
        {"btn0", ui->btn0}, {"btn1", ui->btn1}, {"btn2", ui->btn2},
        {"btn3", ui->btn3}, {"btn4", ui->btn4}, {"btn5", ui->btn5},
        {"btn6", ui->btn6}, {"btn7", ui->btn7}, {"btn8", ui->btn8},
        {"btn9", ui->btn9}
    };
    for (auto it = digitButtons.begin(); it != digitButtons.end(); ++it) {
        connect(it.value(), &QPushButton::clicked, this, &MainWindow::onDigitClicked);
    }

    // 小数点
    connect(ui->btnDot, &QPushButton::clicked, this, &MainWindow::onDotClicked);

    // 四则运算符
    connect(ui->btnAdd, &QPushButton::clicked, this, &MainWindow::onOperatorClicked);
    connect(ui->btnSubtract, &QPushButton::clicked, this, &MainWindow::onOperatorClicked);
    connect(ui->btnMultiply, &QPushButton::clicked, this, &MainWindow::onOperatorClicked);
    connect(ui->btnDivide, &QPushButton::clicked, this, &MainWindow::onOperatorClicked);

    // 等号
    connect(ui->btnEqual, &QPushButton::clicked, this, &MainWindow::onEqualClicked);

    // 清除
    connect(ui->btnC, &QPushButton::clicked, this, &MainWindow::onClearClicked);
    connect(ui->btnCE, &QPushButton::clicked, this, &MainWindow::onClearEntryClicked);
    connect(ui->btnBackspace, &QPushButton::clicked, this, &MainWindow::onBackspaceClicked);

    // 正负号
    connect(ui->btnNegate, &QPushButton::clicked, this, &MainWindow::onNegateClicked);

    // 一元运算
    connect(ui->btnPercent, &QPushButton::clicked, this, &MainWindow::onUnaryOperatorClicked);
    connect(ui->btnReciprocal, &QPushButton::clicked, this, &MainWindow::onUnaryOperatorClicked);
    connect(ui->btnSquare, &QPushButton::clicked, this, &MainWindow::onUnaryOperatorClicked);
    connect(ui->btnCubeRoot, &QPushButton::clicked, this, &MainWindow::onUnaryOperatorClicked);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    const int key = event->key();
    const Qt::KeyboardModifiers modifiers = event->modifiers();

    // 数字键
    if (key >= Qt::Key_0 && key <= Qt::Key_9) {
        inputDigit(QString::number(key - Qt::Key_0));
        return;
    }

    // 小数点（主键盘. 和 小键盘.）
    if (key == Qt::Key_Period || key == Qt::Key_Comma) {
        inputDot();
        return;
    }

    // 运算符
    if (key == Qt::Key_Plus) {
        inputOperator("+");
        return;
    }
    if (key == Qt::Key_Minus) {
        inputOperator("-");
        return;
    }
    if (key == Qt::Key_Asterisk) {
        inputOperator("×");
        return;
    }
    if (key == Qt::Key_Slash) {
        inputOperator("÷");
        return;
    }

    // 等号 / 回车
    if (key == Qt::Key_Equal || key == Qt::Key_Return || key == Qt::Key_Enter) {
        inputEqual();
        return;
    }

    // 退格
    if (key == Qt::Key_Backspace) {
        inputBackspace();
        return;
    }

    // 清除全部 (Delete 或 Esc)
    if (key == Qt::Key_Delete || key == Qt::Key_Escape) {
        inputClear();
        return;
    }

    // 清除当前 (Ctrl+Backspace)
    if (key == Qt::Key_Backspace && modifiers & Qt::ControlModifier) {
        inputClearEntry();
        return;
    }

    // 正负号 (F9)
    if (key == Qt::Key_F9) {
        inputNegate();
        return;
    }

    // 一元运算快捷键
    if (key == Qt::Key_Percent) {
        inputUnaryOperator("%");
        return;
    }
    if (key == Qt::Key_R) {
        inputUnaryOperator("1/x");
        return;
    }
    if (key == Qt::Key_Q) {
        inputUnaryOperator("x²");
        return;
    }

    QMainWindow::keyPressEvent(event);
}

// ==================== 槽函数（按钮点击） ====================

void MainWindow::onDigitClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    inputDigit(btn->text());
}

void MainWindow::onDotClicked()
{
    inputDot();
}

void MainWindow::onOperatorClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    inputOperator(btn->text());
}

void MainWindow::onEqualClicked()
{
    inputEqual();
}

void MainWindow::onClearClicked()
{
    inputClear();
}

void MainWindow::onClearEntryClicked()
{
    inputClearEntry();
}

void MainWindow::onBackspaceClicked()
{
    inputBackspace();
}

void MainWindow::onNegateClicked()
{
    inputNegate();
}

void MainWindow::onUnaryOperatorClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    inputUnaryOperator(btn->text());
}

// ==================== 核心输入处理（鼠标和键盘共用） ====================

void MainWindow::inputDigit(const QString &digit)
{
    // 错误状态下先清除错误
    if (hasError) {
        inputClear();
    }

    // 刚完成计算或等待新操作数，输入数字则开始新输入
    if (waitForOperand) {
        currentInput.clear();
        waitForOperand = false;
    }

    // 避免多个前导0，如 "000" -> "0"
    if (currentInput == "0") {
        currentInput = digit;
    } else if (currentInput == "-0") {
        currentInput = "-" + digit;
    } else {
        currentInput += digit;
    }

    updateDisplay(currentInput);
}

void MainWindow::inputDot()
{
    if (hasError) {
        inputClear();
    }

    // 刚完成计算，输入小数点则清空并以 "0." 开始
    if (waitForOperand) {
        currentInput = "0.";
        waitForOperand = false;
        updateDisplay(currentInput);
        return;
    }

    // 已有小数点则忽略（防止连续小数点）
    if (currentInput.contains('.')) {
        return;
    }

    // 空输入则补0
    if (currentInput.isEmpty()) {
        currentInput = "0.";
    } else {
        currentInput += '.';
    }

    updateDisplay(currentInput);
}

void MainWindow::inputOperator(const QString &op)
{
    // 错误状态下先清除错误，并用0作为起点
    if (hasError) {
        inputClear();
    }

    // 连续按操作符：替换操作符（连续操作符处理）
    if (waitForOperand && !pendingOperator.isEmpty()) {
        pendingOperator = op;
        updateDisplay(formatNumber(firstOperand.toDouble()) + " " + pendingOperator);
        return;
    }

    // 如果已有待计算表达式，先计算结果（连续运算）
    if (!pendingOperator.isEmpty() && !currentInput.isEmpty()) {
        double result;
        if (!calculate(result)) {
            return; // 计算出错（如除0）
        }
        firstOperand = QString::number(result);
        currentInput = firstOperand;
        updateDisplay(formatNumber(result) + " " + op);
    } else {
        // 首次输入操作符，保存当前值为第一操作数
        firstOperand = currentInput;
        updateDisplay(formatNumber(firstOperand.toDouble()) + " " + op);
    }

    pendingOperator = op;
    waitForOperand = true;
}

void MainWindow::inputEqual()
{
    if (hasError) {
        return;
    }

    // 没有待计算的操作符，直接忽略
    if (pendingOperator.isEmpty()) {
        return;
    }

    // 等待第二操作数时按等号，用第一操作数作为第二操作数（如 "5 + =" -> 10）
    QString secondOperand = currentInput;
    if (waitForOperand) {
        secondOperand = firstOperand;
    }

    double result;
    if (!calculate(result)) {
        return; // 除0错误已在calculate中处理
    }

    // 显示结果，并标记等待新输入（计算完成后继续输入数字会开始新计算）
    updateDisplay(formatNumber(result));
    currentInput = QString::number(result);
    firstOperand.clear();
    pendingOperator.clear();
    waitForOperand = true;
}

void MainWindow::inputBackspace()
{
    // 错误状态或刚完成计算，退格等同于清除全部
    if (hasError || waitForOperand) {
        inputClear();
        return;
    }

    // 删除最后一个字符
    if (!currentInput.isEmpty()) {
        currentInput.chop(1);
    }

    // 删空后显示0
    if (currentInput.isEmpty() || currentInput == "-") {
        currentInput = "0";
    }

    updateDisplay(currentInput);
}

void MainWindow::inputClear()
{
    // 完全重置所有状态
    currentInput = "0";
    firstOperand.clear();
    pendingOperator.clear();
    waitForOperand = false;
    hasError = false;
    updateDisplay(currentInput);
}

void MainWindow::inputClearEntry()
{
    // 只清除当前输入，保留待计算表达式
    if (hasError) {
        inputClear();
        return;
    }
    currentInput = "0";
    updateDisplay(currentInput);
}

void MainWindow::inputNegate()
{
    if (hasError) {
        return;
    }

    // 等待输入时切换正负号，先取出第一操作数
    if (waitForOperand) {
        currentInput = firstOperand;
        waitForOperand = false;
    }

    // 0不处理正负号
    if (currentInput == "0" || currentInput.isEmpty()) {
        return;
    }

    // 切换符号
    if (currentInput.startsWith('-')) {
        currentInput.remove(0, 1);
    } else {
        currentInput.prepend('-');
    }

    updateDisplay(currentInput);
}

void MainWindow::inputUnaryOperator(const QString &op)
{
    if (hasError) {
        return;
    }

    double value = currentInput.toDouble();

    if (op == "%") {
        value = value / 100.0;
    } else if (op == "1/x") {
        if (qFuzzyIsNull(value)) {
            hasError = true;
            updateDisplay(QStringLiteral("除数不能为零"));
            return;
        }
        value = 1.0 / value;
    } else if (op == "x²") {
        value = value * value;
    } else if (op == "³√x") {
        value = std::cbrt(value);
    }

    currentInput = formatNumber(value);
    waitForOperand = true; // 一元运算后视为完成一次计算
    updateDisplay(currentInput);
}

// ==================== 辅助函数 ====================

bool MainWindow::calculate(double &result)
{
    bool ok1, ok2;
    double a = firstOperand.toDouble(&ok1);
    double b = currentInput.toDouble(&ok2);

    if (!ok1 || !ok2) {
        hasError = true;
        updateDisplay(QStringLiteral("错误"));
        return false;
    }

    if (pendingOperator == "+") {
        result = a + b;
    } else if (pendingOperator == "-") {
        result = a - b;
    } else if (pendingOperator == "×") {
        result = a * b;
    } else if (pendingOperator == "÷") {
        if (qFuzzyIsNull(b)) {
            hasError = true;
            updateDisplay(QStringLiteral("除数不能为零"));
            return false;
        }
        result = a / b;
    } else {
        result = b;
    }

    return true;
}

void MainWindow::updateDisplay(const QString &text)
{
    ui->displayEdit->setText(text);
}

QString MainWindow::formatNumber(double value)
{
    // 处理无穷大和NaN
    if (std::isinf(value)) {
        return value > 0 ? QStringLiteral("∞") : QStringLiteral("-∞");
    }
    if (std::isnan(value)) {
        return QStringLiteral("错误");
    }

    // 使用QString::number自动处理精度，'g'格式去除末尾多余的0
    QString str = QString::number(value, 'g', 15);

    // 修复科学计数法中不必要的正号，如 "1e+05" -> "1e+05"（保持原样即可）
    // 但把 "0e+00" 转为 "0"
    if (str.contains("e+00") || str.contains("e-00")) {
        double v = str.toDouble();
        if (qFuzzyIsNull(v)) {
            return "0";
        }
    }

    return str;
}
