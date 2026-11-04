#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "codeeditor.h"
#include "finddialog.h"
#include "ui_finddialog.h"
#include "replacedialog.h"
#include "ui_replacedialog.h"
#include "aboutdialog.h"

#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QFontDialog>
#include <QColorDialog>
#include <QPalette>
#include <QClipboard>
#include <QApplication>
#include <QTextCursor>
#include <QCloseEvent>
#include <QTimer>
#include <QFileInfo>

// 定时自动保存间隔（毫秒）
static const int kAutoSaveIntervalMs = 30 * 1000;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_autoSaveTimer(new QTimer(this))
{
    ui->setupUi(this);
    ui->toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);

    // ========== 初始化成员状态 ==========
    m_currentFilePath.clear();
    updateWindowTitle();

    // 查看菜单的两个可勾选 Action，默认勾选（工具栏和状态栏默认显示）
    ui->actionToggleToolbar->setChecked(true);
    ui->actionToggleStatusbar->setChecked(true);

    // 默认不自动换行
    ui->textEdit->setLineWrapMode(QPlainTextEdit::NoWrap);

    // ========== 文件操作 Action 连接 ==========
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::onNewFile);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onOpenFile);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onSaveFile);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::onSaveFileAs);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::onQuit);

    // ========== 编辑操作 Action 连接 ==========
    connect(ui->actionUndo, &QAction::triggered, this, &MainWindow::onUndo);
    connect(ui->actionRedo, &QAction::triggered, this, &MainWindow::onRedo);
    connect(ui->actionCut, &QAction::triggered, this, &MainWindow::onCut);
    connect(ui->actionCopy, &QAction::triggered, this, &MainWindow::onCopy);
    connect(ui->actionPaste, &QAction::triggered, this, &MainWindow::onPaste);
    connect(ui->actionSelectAll, &QAction::triggered, this, &MainWindow::onSelectAll);

    // ========== 格式操作 Action 连接 ==========
    connect(ui->actionAutoWrap, &QAction::triggered, this, &MainWindow::onAutoWrap);
    connect(ui->actionShowLineNumbers, &QAction::triggered,
            this, &MainWindow::onShowLineNumbers);
    connect(ui->actionFont, &QAction::triggered, this, &MainWindow::onFont);
    connect(ui->actionFontColor, &QAction::triggered, this, &MainWindow::onFontColor);
    connect(ui->agBgColor, &QAction::triggered, this, &MainWindow::onTextBgColor);
    connect(ui->actionEditBg, &QAction::triggered, this, &MainWindow::onEditBgColor);

    // ========== 查看操作 Action 连接 ==========
    connect(ui->actionToggleToolbar, &QAction::triggered, this, &MainWindow::onToggleToolbar);
    connect(ui->actionToggleStatusbar, &QAction::triggered, this, &MainWindow::onToggleStatusbar);
    connect(ui->actionAutoSave, &QAction::triggered,
            this, &MainWindow::onToggleAutoSave);

    // ========== 帮助 Action ==========
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);

    // ========== 查找 / 替换 Action ==========
    connect(ui->actionFind, &QAction::triggered, this, &MainWindow::onFind);
    connect(ui->actionReplace, &QAction::triggered, this, &MainWindow::onReplace);

    // ========== textEdit 信号连接 ==========
    // 复制/剪切按钮使能
    connect(ui->textEdit, &CodeEditor::copyAvailable,
            ui->actionCut, &QAction::setEnabled);
    connect(ui->textEdit, &CodeEditor::copyAvailable,
            ui->actionCopy, &QAction::setEnabled);

    // 撤销/重做按钮使能
    connect(ui->textEdit, &CodeEditor::undoAvailable,
            ui->actionUndo, &QAction::setEnabled);
    connect(ui->textEdit, &CodeEditor::redoAvailable,
            ui->actionRedo, &QAction::setEnabled);

    // 剪贴板变化时更新粘贴按钮使能
    connect(QApplication::clipboard(), &QClipboard::dataChanged,
            this, &MainWindow::onClipboardChanged);
    // 初始化一次粘贴按钮状态
    onClipboardChanged();

    // 状态栏实时更新
    connect(ui->textEdit, &CodeEditor::blockCountChanged,
            this, &MainWindow::updateStatusBar);
    connect(ui->textEdit, &CodeEditor::cursorPositionChanged,
            this, &MainWindow::updateStatusBar);
    connect(ui->textEdit, &CodeEditor::textChanged,
            this, &MainWindow::updateStatusBar);
    // 初始化状态栏
    updateStatusBar();

    // 文档修改状态 -> 窗口标题
    connect(ui->textEdit->document(), &QTextDocument::modificationChanged,
            this, &MainWindow::onDocumentModified);

    // ========== 定时自动保存 ==========
    m_autoSaveTimer->setInterval(kAutoSaveIntervalMs);
    connect(m_autoSaveTimer, &QTimer::timeout, this, &MainWindow::autoSave);
    // 默认开启
    m_autoSaveTimer->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// =====================================================================
// 关闭事件：若文档已修改，弹出对话框询问
// =====================================================================
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (ui->textEdit->document()->isModified()) {
        auto ret = QMessageBox::question(
            this,
            QStringLiteral("保存更改？"),
            QStringLiteral("文档已被修改，是否保存更改？"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
            QMessageBox::Save);

        if (ret == QMessageBox::Save) {
            // 尝试保存：保存成功则 accept，用户取消/失败则 ignore
            if (onSaveFile()) {
                event->accept();
            } else {
                event->ignore();
            }
        } else if (ret == QMessageBox::Discard) {
            event->accept();
        } else {
            // Cancel：不关闭
            event->ignore();
        }
    } else {
        event->accept();
    }
}

// =====================================================================
// 文件操作
// =====================================================================
void MainWindow::onNewFile()
{
    // 如果文档已修改，先提示保存
    if (ui->textEdit->document()->isModified()) {
        auto ret = QMessageBox::question(
            this,
            QStringLiteral("新建文件"),
            QStringLiteral("当前文档已修改，是否保存更改？"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
            QMessageBox::Save);

        if (ret == QMessageBox::Save) {
            if (!saveFile(m_currentFilePath)) {
                // 用户取消了保存
                return;
            }
        } else if (ret == QMessageBox::Cancel) {
            return;
        }
    }

    ui->textEdit->clear();
    m_currentFilePath.clear();
    ui->textEdit->document()->setModified(false);
    updateWindowTitle();
}

void MainWindow::onOpenFile()
{
    // 如果文档已修改，先提示保存
    if (ui->textEdit->document()->isModified()) {
        auto ret = QMessageBox::question(
            this,
            QStringLiteral("打开文件"),
            QStringLiteral("当前文档已修改，是否保存更改？"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
            QMessageBox::Save);

        if (ret == QMessageBox::Save) {
            if (!saveFile(m_currentFilePath)) {
                return;
            }
        } else if (ret == QMessageBox::Cancel) {
            return;
        }
    }

    QString path = QFileDialog::getOpenFileName(
        this,
        QStringLiteral("打开文本文件"),
        QString(),
        QStringLiteral("文本文件 (*.txt);;所有文件 (*)"));

    if (path.isEmpty()) {
        return;
    }

    if (loadFile(path)) {
        m_currentFilePath = path;
        ui->textEdit->document()->setModified(false);
        updateWindowTitle();
    }
}

bool MainWindow::onSaveFile()
{
    if (m_currentFilePath.isEmpty()) {
        // 走另存为
        QString path = QFileDialog::getSaveFileName(
            this,
            QStringLiteral("另存为"),
            QStringLiteral("新建文本文件.txt"),
            QStringLiteral("文本文件 (*.txt);;所有文件 (*)"));

        if (path.isEmpty()) {
            // 用户取消
            return false;
        }
        m_currentFilePath = path;
    }

    if (!saveFile(m_currentFilePath)) {
        return false;
    }

    ui->textEdit->document()->setModified(false);
    updateWindowTitle();
    return true;
}

void MainWindow::onSaveFileAs()
{
    QString path = QFileDialog::getSaveFileName(
        this,
        QStringLiteral("另存为"),
        m_currentFilePath.isEmpty()
            ? QStringLiteral("新建文本文件.txt")
            : m_currentFilePath,
        QStringLiteral("文本文件 (*.txt);;所有文件 (*)"));

    if (path.isEmpty()) {
        return;
    }

    if (saveFile(path)) {
        m_currentFilePath = path;
        ui->textEdit->document()->setModified(false);
        updateWindowTitle();
    }
}

void MainWindow::onQuit()
{
    close();
}

// =====================================================================
// 编辑操作
// =====================================================================
void MainWindow::onUndo()      { ui->textEdit->undo(); }
void MainWindow::onRedo()      { ui->textEdit->redo(); }
void MainWindow::onCut()       { ui->textEdit->cut(); }
void MainWindow::onCopy()      { ui->textEdit->copy(); }
void MainWindow::onPaste()     { ui->textEdit->paste(); }
void MainWindow::onSelectAll() { ui->textEdit->selectAll(); }

// =====================================================================
// 格式操作
// =====================================================================
void MainWindow::onAutoWrap()
{
    // 在 NoWrap 和 WidgetWidth 之间切换
    // QPlainTextEdit 没有 WordWrap，对应枚举是 WidgetWidth
    if (ui->textEdit->lineWrapMode() == QPlainTextEdit::NoWrap) {
        ui->textEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    } else {
        ui->textEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
    }
}

void MainWindow::onShowLineNumbers()
{
    // 切换行号栏显示
    ui->textEdit->setLineNumberAreaVisible(
        ui->actionShowLineNumbers->isChecked());
}

void MainWindow::onFont()
{
    bool ok = false;
    QFont font = QFontDialog::getFont(&ok, ui->textEdit->font(), this,
                                      QStringLiteral("选择字体"));
    if (ok) {
        ui->textEdit->setFont(font);
    }
}

void MainWindow::onFontColor()
{
    QColor color = QColorDialog::getColor(
        Qt::black, this, QStringLiteral("选择字体颜色"));
    if (!color.isValid()) {
        return;
    }

    // QPlainTextEdit 没有 setTextColor，需要用 QTextCharFormat 批量设置
    QTextCharFormat fmt;
    fmt.setForeground(color);

    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.select(QTextCursor::Document);
    cursor.mergeCharFormat(fmt);
}

void MainWindow::onTextBgColor()
{
    QColor color = QColorDialog::getColor(
        Qt::yellow, this, QStringLiteral("选择字体背景色"));
    if (!color.isValid()) {
        return;
    }

    // 同上：用 QTextCharFormat 设置字符背景色
    QTextCharFormat fmt;
    fmt.setBackground(color);

    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.select(QTextCursor::Document);
    cursor.mergeCharFormat(fmt);
}

void MainWindow::onEditBgColor()
{
    QColor color = QColorDialog::getColor(
        ui->textEdit->palette().color(QPalette::Base), this,
        QStringLiteral("选择编辑器背景色"));
    if (color.isValid()) {
        QPalette pal = ui->textEdit->palette();
        pal.setColor(QPalette::Base, color);
        ui->textEdit->setPalette(pal);
    }
}

// =====================================================================
// 查看操作
// =====================================================================
void MainWindow::onToggleToolbar()
{
    ui->toolBar->setVisible(ui->actionToggleToolbar->isChecked());
}

void MainWindow::onToggleStatusbar()
{
    statusBar()->setVisible(ui->actionToggleStatusbar->isChecked());
}

void MainWindow::onToggleAutoSave()
{
    // 根据 Action 勾选状态启停定时器
    if (ui->actionAutoSave->isChecked()) {
        if (!m_autoSaveTimer->isActive()) {
            m_autoSaveTimer->start();
        }
    } else {
        if (m_autoSaveTimer->isActive()) {
            m_autoSaveTimer->stop();
        }
    }
}

// =====================================================================
// 帮助
// =====================================================================
void MainWindow::onAbout()
{
    AboutDialog dlg(this);
    dlg.exec();
}

// =====================================================================
// 查找 / 替换
// =====================================================================
void MainWindow::onFind()
{
    FindDialog dlg(this);

    // 预先设置默认方向：向下
    dlg.ui->radioDown->setChecked(true);

    if (dlg.exec() != QDialog::Accepted) {
        return;
    }

    QString searchText = dlg.ui->findEdit->text();
    if (searchText.isEmpty()) {
        return;
    }

    QTextDocument::FindFlags flags;
    if (dlg.ui->radioUp->isChecked()) {
        flags |= QTextDocument::FindBackward;
    }
    if (dlg.ui->caseSensitiveCheck->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }

    if (!findText(searchText, flags)) {
        QMessageBox::information(this, QStringLiteral("查找"),
                                 QStringLiteral("找不到\"%1\"").arg(searchText));
    }
}

void MainWindow::onReplace()
{
    ReplaceDialog dlg(this);

    if (dlg.exec() != QDialog::Accepted) {
        return;
    }

    QString searchText  = dlg.ui->findEdit->text();
    QString replaceText = dlg.ui->replaceEdit->text();
    if (searchText.isEmpty()) {
        return;
    }

    QTextDocument::FindFlags flags;
    if (dlg.ui->radioUp->isChecked()) {
        flags |= QTextDocument::FindBackward;
    }
    if (dlg.ui->caseSensitiveCheck->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }

    int clicked = dlg.result();

    if (clicked == 1) {
        // ===== 查找下一个 =====
        if (!findText(searchText, flags)) {
            QMessageBox::information(this, QStringLiteral("替换"),
                                     QStringLiteral("找不到\"%1\"").arg(searchText));
        }
    } else if (clicked == 2) {
        // ===== 替换当前选中 + 查找下一个 =====
        QTextCursor cursor = ui->textEdit->textCursor();
        if (cursor.hasSelection() && cursor.selectedText() == searchText) {
            cursor.beginEditBlock();
            cursor.removeSelectedText();
            cursor.insertText(replaceText);
            cursor.endEditBlock();
        }
        // 替换后自动查找下一个
        if (!findText(searchText, flags)) {
            QMessageBox::information(this, QStringLiteral("替换"),
                                     QStringLiteral("找不到\"%1\"").arg(searchText));
        }
    } else if (clicked == 3) {
        // ===== 全部替换 =====
        // 用 QTextDocument::find + 显式游标逐段查找替换，
        // QTextDocument::find 不会自动绕回，循环必然能正常结束
        QTextCursor cursor(ui->textEdit->document());
        cursor.movePosition(flags.testFlag(QTextDocument::FindBackward)
                                ? QTextCursor::End
                                : QTextCursor::Start);

        int replaceCount = 0;
        while (true) {
            QTextCursor found = ui->textEdit->document()->find(
                searchText, cursor, flags);
            if (found.isNull()) {
                break;
            }

            found.beginEditBlock();
            found.removeSelectedText();
            found.insertText(replaceText);
            found.endEditBlock();

            // 从替换后的位置继续向后查找
            cursor = found;
            ++replaceCount;
        }

        if (replaceCount == 0) {
            QMessageBox::information(this, QStringLiteral("替换"),
                                     QStringLiteral("找不到\"%1\"").arg(searchText));
        } else {
            QMessageBox::information(this, QStringLiteral("替换"),
                                     QStringLiteral("共替换了 %1 处").arg(replaceCount));
        }
    }
}

// =====================================================================
// 查找辅助：QPlainTextEdit::find 从当前光标处开始且不会绕回，
// 所以第一次失败时把光标移到文档开头（或末尾）再找一次
// =====================================================================
bool MainWindow::findText(const QString &text, QTextDocument::FindFlags flags)
{
    if (ui->textEdit->find(text, flags)) {
        return true;
    }

    // 绕回：向上查找移到文档末尾，向下查找移到文档开头
    QTextCursor cursor(ui->textEdit->document());
    cursor.movePosition(flags.testFlag(QTextDocument::FindBackward)
                            ? QTextCursor::End
                            : QTextCursor::Start);
    ui->textEdit->setTextCursor(cursor);

    return ui->textEdit->find(text, flags);
}

// =====================================================================
// 状态栏更新
// =====================================================================
void MainWindow::updateStatusBar()
{
    CodeEditor *edit = ui->textEdit;
    QString text = edit->toPlainText();

    // 长度（字符数）
    int length = text.length();
    // 行数
    int lines = edit->blockCount();
    // 当前行列
    QTextCursor cursor = edit->textCursor();
    int curLine = cursor.blockNumber() + 1;       // 从 1 开始
    int curCol  = cursor.columnNumber() + 1;      // 从 1 开始

    statusBar()->showMessage(
        QStringLiteral("length: %1  lines: %2  Ln: %3  Col: %4")
            .arg(length)
            .arg(lines)
            .arg(curLine)
            .arg(curCol));
}

// =====================================================================
// 窗口标题联动
// =====================================================================
void MainWindow::onDocumentModified(bool modified)
{
    Q_UNUSED(modified);
    updateWindowTitle();
}

void MainWindow::updateWindowTitle()
{
    QString fileName;
    if (m_currentFilePath.isEmpty()) {
        fileName = QStringLiteral("新建文本文件");
    } else {
        // 取文件名（不含路径）
        QFileInfo fi(m_currentFilePath);
        fileName = fi.fileName();
    }

    QString title = QStringLiteral("%1 - 编辑器").arg(fileName);
    if (ui->textEdit->document()->isModified()) {
        title.prepend('*');
    }
    setWindowTitle(title);
}

// =====================================================================
// 粘贴按钮使能
// =====================================================================
void MainWindow::onClipboardChanged()
{
    ui->actionPaste->setEnabled(
        !QApplication::clipboard()->text().isEmpty());
}

// =====================================================================
// 定时自动保存（静默保存，不弹对话框）
// =====================================================================
void MainWindow::autoSave()
{
    // 只有文档已修改且有保存过的路径时才自动保存
    if (!ui->textEdit->document()->isModified()) {
        return;
    }
    if (m_currentFilePath.isEmpty()) {
        return;
    }

    // 静默保存（saveFile 内部也不弹对话框，失败只会 return false）
    if (saveFile(m_currentFilePath)) {
        ui->textEdit->document()->setModified(false);
        updateWindowTitle();
    }
    // 自动保存失败时保持静默，等下一轮定时器再试
}

// =====================================================================
// 文件读写辅助函数
// =====================================================================
bool MainWindow::loadFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, QStringLiteral("打开文件"),
                             QStringLiteral("无法打开文件：%1\n%2")
                                 .arg(path, file.errorString()));
        return false;
    }

    QTextStream in(&file);
    // 使用系统默认编码读取
    in.setEncoding(QStringConverter::Utf8);
    QString content = in.readAll();
    file.close();

    ui->textEdit->setPlainText(content);
    return true;
}

bool MainWindow::saveFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, QStringLiteral("保存文件"),
                             QStringLiteral("无法写入文件：%1\n%2")
                                 .arg(path, file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << ui->textEdit->toPlainText();
    file.close();

    return true;
}
