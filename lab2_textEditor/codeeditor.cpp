#include "codeeditor.h"

#include <QPainter>
#include <QTextBlock>
#include <QResizeEvent>
#include <QScrollBar>

// =====================================================================
// LineNumberArea
// =====================================================================
LineNumberArea::LineNumberArea(CodeEditor *editor)
    : QWidget(editor)
    , m_codeEditor(editor)
{
}

void LineNumberArea::paintEvent(QPaintEvent *event)
{
    // 直接委托给 CodeEditor 绘制
    m_codeEditor->lineNumberAreaPaintEvent(event);
}

QSize LineNumberArea::sizeHint() const
{
    return QSize(m_codeEditor->lineNumberAreaWidth(), 0);
}

// =====================================================================
// CodeEditor
// =====================================================================
CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent)
    , m_lineNumberArea(new LineNumberArea(this))
    , m_lineNumberAreaVisible(true)
{
    // 行号区域刷新信号
    connect(this, &CodeEditor::blockCountChanged,
            this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest,
            this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged,
            this, &CodeEditor::onCursorPosChanged);

    // 初始设置边距 & 显示行号区域
    updateLineNumberAreaWidth(0);
    m_lineNumberArea->show();
}

int CodeEditor::lineNumberAreaWidth()
{
    if (!m_lineNumberAreaVisible) {
        return 0;
    }

    // 计算当前最大行号有多少位（例如 1000 行 → 4 位）
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    // 每一位数字宽度 + 左右 padding（共 20px）
    int space = 10 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void CodeEditor::setLineNumberAreaVisible(bool visible)
{
    m_lineNumberAreaVisible = visible;
    m_lineNumberArea->setVisible(visible);
    updateLineNumberAreaWidth(blockCount());
}

bool CodeEditor::isLineNumberAreaVisible() const
{
    return m_lineNumberAreaVisible;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy) {
        m_lineNumberArea->scroll(0, dy);
    } else {
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth(0);
    }
}

void CodeEditor::onCursorPosChanged()
{
    // 光标改变时重新绘制行号区域（用于当前行高亮刷新）
    if (m_lineNumberAreaVisible) {
        m_lineNumberArea->update();
    }
}

void CodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);

    if (m_lineNumberAreaVisible) {
        QRect cr = contentsRect();
        m_lineNumberArea->setGeometry(
            QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
    }
}

void CodeEditor::scrollContentsBy(int dx, int dy)
{
    QPlainTextEdit::scrollContentsBy(dx, dy);
    if (dy != 0 && m_lineNumberAreaVisible) {
        m_lineNumberArea->scroll(0, dy);
    }
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    if (!m_lineNumberAreaVisible) {
        return;
    }

    QPainter painter(m_lineNumberArea);
    painter.fillRect(event->rect(), QColor(0xFFFFCC));  // 浅黄色背景

    // 遍历可见的代码块，画行号
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top    = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    const int currentBlockNumber = textCursor().blockNumber();
    const int areaWidth = m_lineNumberArea->width();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            // 当前行号高亮（浅橙色）
            if (blockNumber == currentBlockNumber) {
                painter.fillRect(
                    QRect(0, top, areaWidth,
                          qRound(blockBoundingRect(block).height())),
                    QColor(0xFFFF99));
            }

            // 行号文字：深灰色，右对齐
            painter.setPen(blockNumber == currentBlockNumber
                               ? QColor(0xCC0000)   // 当前行：红色
                               : QColor(0x555555)); // 其他行：深灰

            QString number = QString::number(blockNumber + 1);
            painter.drawText(0, top, areaWidth, fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}
