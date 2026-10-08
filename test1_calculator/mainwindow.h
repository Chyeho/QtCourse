#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QPushButton>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    // 数字输入
    void onDigitClicked();
    // 小数点
    void onDotClicked();
    // 操作符（+ - × ÷）
    void onOperatorClicked();
    // 等号
    void onEqualClicked();
    // 清除全部 (C)
    void onClearClicked();
    // 清除当前输入 (CE)
    void onClearEntryClicked();
    // 退格
    void onBackspaceClicked();
    // 正负号切换
    void onNegateClicked();
    // 一元运算：%、1/x、x²、³√x
    void onUnaryOperatorClicked();

private:
    // 初始化按钮与槽函数的映射
    void setupConnections();
    // 处理数字输入（键盘和鼠标共用）
    void inputDigit(const QString &digit);
    // 处理小数点输入（键盘和鼠标共用）
    void inputDot();
    // 处理操作符（键盘和鼠标共用）
    void inputOperator(const QString &op);
    // 处理等号（键盘和鼠标共用）
    void inputEqual();
    // 处理退格（键盘和鼠标共用）
    void inputBackspace();
    // 处理清除全部（键盘和鼠标共用）
    void inputClear();
    // 处理清除当前（键盘和鼠标共用）
    void inputClearEntry();
    // 处理正负号（键盘和鼠标共用）
    void inputNegate();
    // 处理一元运算（键盘和鼠标共用）
    void inputUnaryOperator(const QString &op);
    // 计算二元运算结果
    bool calculate(double &result);
    // 更新显示
    void updateDisplay(const QString &text);
    // 格式化数字显示，去除多余的0
    QString formatNumber(double value);

    Ui::MainWindow *ui;

    // 当前输入的字符串
    QString currentInput;
    // 第一个操作数（字符串形式，用于连续运算）
    QString firstOperand;
    // 当前操作符
    QString pendingOperator;
    // 是否刚完成计算（按了等号或一元运算），用于判断下一次输入是否清空
    bool waitForOperand;
    // 错误状态标志
    bool hasError;
    // 记录一元运算后的值，用于连续一元运算
    double unaryResult;
};

#endif // MAINWINDOW_H
