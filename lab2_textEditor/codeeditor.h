#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QWidget>

// 行号区域控件：CodeEditor 左侧的黄色栏
class LineNumberArea : public QWidget
{
    Q_OBJECT

public:
    explicit LineNumberArea(class CodeEditor *editor);

    // 供 CodeEditor 调用的绘制接口
    void paintEvent(QPaintEvent *event) override;
    // 计算建议尺寸
    QSize sizeHint() const override;

private:
    class CodeEditor *m_codeEditor;
};

// 带行号栏的编辑器，继承 QPlainTextEdit
class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);

    // 行号区域宽度（按行数位数动态计算）
    int lineNumberAreaWidth();

    // 获取 / 设置行号区域可见性
    void setLineNumberAreaVisible(bool visible);
    bool isLineNumberAreaVisible() const;

    // 行号区域绘制（由 LineNumberArea::paintEvent 调用）
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    // 行号区域控件指针
    LineNumberArea *lineNumberArea() const { return m_lineNumberArea; }

protected:
    void resizeEvent(QResizeEvent *event) override;
    void scrollContentsBy(int dx, int dy) override;

private slots:
    // blockCountChanged 时更新 viewport 左边距
    void updateLineNumberAreaWidth(int newBlockCount);
    // updateRequest 时滚动行号区域
    void updateLineNumberArea(const QRect &rect, int dy);
    // 光标移动时触发重绘（用于当前行高亮）
    void onCursorPosChanged();

private:
    LineNumberArea *m_lineNumberArea;
    bool            m_lineNumberAreaVisible;
};

#endif // CODEEDITOR_H
