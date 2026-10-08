#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentInput("0")
    , waitForOperand(false)
    , secondOperandEntered(true)
    , hasError(false)
{
    ui->setupUi(this);
    setupConnections();
    updateDisplay(currentInput);
    updateExpression(QString());
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

    // 清除当前 (Ctrl+Backspace) —— 必须放在 Backspace 判断之前，否则永远不可达
    if (key == Qt::Key_Backspace && modifiers & Qt::ControlModifier) {
        inputClearEntry();
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
//
// 显示屏采用 Windows 计算器双行布局：
//   - expressionEdit（上行，小字号）：显示历史表达式，如 "9 ×"、"9 × 9 ="
//   - displayEdit（下行，大字号，右下角）：显示当前输入值或计算结果

void MainWindow::inputDigit(const QString &digit)
{
    // 错误状态下先清除错误
    if (hasError) {
        inputClear();
    }

    // 刚按过操作符/等号：输入数字则替换当前值，开始新输入
    if (waitForOperand) {
        currentInput.clear();
        waitForOperand = false;
        // 等号后开始新计算时清空表达式行；操作符后保留表达式（如 "9 ×"）
        if (pendingOperator.isEmpty()) {
            updateExpression(QString());
        }
    }
    secondOperandEntered = true;

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

    // 刚按过操作符/等号：以 "0." 开始新输入
    if (waitForOperand) {
        currentInput = "0.";
        waitForOperand = false;
        secondOperandEntered = true;
        if (pendingOperator.isEmpty()) {
            updateExpression(QString());
        }
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

    secondOperandEntered = true;
    updateDisplay(currentInput);
}

void MainWindow::inputOperator(const QString &op)
{
    // 错误状态下先清除错误，用0作为起点继续
    if (hasError) {
        inputClear();
    }

    if (!pendingOperator.isEmpty() && (!waitForOperand || secondOperandEntered)) {
        // 已有有效表达式且当前值可用：先算出结果（连续运算），结果作为新的第一操作数
        double result;
        if (!calculate(result)) {
            return; // 计算出错（如除0）
        }
        firstOperand = QString::number(result);
        currentInput = firstOperand;
        updateDisplay(formatNumber(result));
    } else if (pendingOperator.isEmpty()) {
        // 首次输入操作符（或等号后接操作符）：当前值即为第一操作数
        firstOperand = currentInput;
    }
    // 其余情况（连续按操作符且未输入第二操作数）：仅替换操作符，firstOperand 不变

    pendingOperator = op;
    // 表达式行显示第一操作数和操作符，如 "9 ×"；当前值行保持不变
    updateExpression(formatNumber(firstOperand.toDouble()) + " " + op);
    waitForOperand = true;
    secondOperandEntered = false;
}

void MainWindow::inputEqual()
{
    if (hasError || pendingOperator.isEmpty()) {
        return; // 无待计算表达式时等号无效
    }

    double result;
    if (!calculate(result)) {
        return; // 除0错误已在calculate中处理
    }

    // 表达式行显示完整等式，如 "9 × 9 ="（currentInput 为第二操作数的原始输入）
    updateExpression(formatNumber(firstOperand.toDouble()) + " "
                     + pendingOperator + " " + currentInput + " =");
    updateDisplay(formatNumber(result));

    currentInput = QString::number(result);
    firstOperand.clear();
    pendingOperator.clear();
    // 等号后再输入数字将开始新计算；再按操作符则以结果为第一操作数
    waitForOperand = true;
    secondOperandEntered = true;
}

void MainWindow::inputBackspace()
{
    // 错误状态下退格等同于清除全部
    if (hasError) {
        inputClear();
        return;
    }

    if (waitForOperand && !secondOperandEntered) {
        // 操作符后尚未输入第二操作数：当前值置0，表达式行保留（Windows 行为）
        currentInput = "0";
        secondOperandEntered = true;
    } else {
        // 正常编辑：删除最后一个字符
        if (!currentInput.isEmpty()) {
            currentInput.chop(1);
        }
        if (currentInput.isEmpty() || currentInput == "-") {
            currentInput = "0";
        }
        waitForOperand = false;
        // 等号后开始编辑结果时清空表达式行
        if (pendingOperator.isEmpty()) {
            updateExpression(QString());
        }
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
    secondOperandEntered = true;
    hasError = false;
    updateDisplay(currentInput);
    updateExpression(QString());
}

void MainWindow::inputClearEntry()
{
    if (hasError) {
        inputClear();
        return;
    }

    // 等号后 CE 相当于全部清除
    if (waitForOperand && pendingOperator.isEmpty()) {
        inputClear();
        return;
    }

    // 只清除当前输入为0，保留待计算表达式（如 "9 ×"）
    currentInput = "0";
    secondOperandEntered = true;
    updateDisplay(currentInput);
}

void MainWindow::inputNegate()
{
    if (hasError) {
        return;
    }

    // 操作符后直接按正负号：取第一操作数作为当前值继续编辑
    if (waitForOperand && !firstOperand.isEmpty()) {
        currentInput = firstOperand;
        waitForOperand = false;
    }
    // 等号后 firstOperand 为空，直接对结果取负

    // 0不处理正负号
    if (currentInput.isEmpty() || currentInput == "0") {
        return;
    }

    // 切换符号
    if (currentInput.startsWith('-')) {
        currentInput.remove(0, 1);
    } else {
        currentInput.prepend('-');
    }

    secondOperandEntered = true;
    updateDisplay(currentInput);
}

void MainWindow::inputUnaryOperator(const QString &op)
{
    if (hasError) {
        return;
    }

    // 操作符后未输入第二操作数时按一元运算：对第一操作数运算
    if (waitForOperand && !firstOperand.isEmpty() && !secondOperandEntered) {
        currentInput = firstOperand;
        waitForOperand = false;
    }

    double value = currentInput.toDouble();

    if (op == "%") {
        value = value / 100.0;
    } else if (op == "1/x") {
        if (qFuzzyIsNull(value)) {
            hasError = true;
            updateDisplay(QStringLiteral("除数不能为零"));
            updateExpression(QString());
            return;
        }
        value = 1.0 / value;
    } else if (op == "x²") {
        value = value * value;
    } else if (op == "³√x") {
        value = std::cbrt(value);
    }

    currentInput = formatNumber(value);
    updateDisplay(currentInput);
    // 一元运算后当前值已就绪：等待后续等号/操作符/新数字
    waitForOperand = true;
    secondOperandEntered = true;
    // 无待计算表达式时清空表达式行
    if (pendingOperator.isEmpty()) {
        updateExpression(QString());
    }
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
        updateExpression(QString());
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
            updateExpression(QString());
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

void MainWindow::updateExpression(const QString &text)
{
    ui->expressionEdit->setText(text);
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

    // 把科学计数法表示的0转回 "0"，如 "0e+00"
    if (str.contains("e+00") || str.contains("e-00")) {
        double v = str.toDouble();
        if (qFuzzyIsNull(v)) {
            return "0";
        }
    }

    return str;
}
