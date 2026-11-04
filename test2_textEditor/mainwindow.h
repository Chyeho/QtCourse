#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    // 关闭事件：若文档已修改则提示保存
    void closeEvent(QCloseEvent *event) override;

private slots:
    // === 文件操作 ===
    void onNewFile();      // 新建
    void onOpenFile();     // 打开
    bool onSaveFile();     // 保存（返回 true 表示成功 / false 表示用户取消或失败）
    void onSaveFileAs();   // 另存为
    void onQuit();         // 退出

    // === 编辑操作 ===
    void onUndo();
    void onRedo();
    void onCut();
    void onCopy();
    void onPaste();
    void onSelectAll();

    // === 格式操作 ===
    void onAutoWrap();          // 自动换行
    void onFont();              // 字体
    void onFontColor();         // 字体颜色
    void onTextBgColor();       // 字体背景色
    void onEditBgColor();       // 编辑器背景色

    // === 查看操作 ===
    void onToggleToolbar();
    void onToggleStatusbar();

    // === 帮助 ===
    void onAbout();

    // === 查找 / 替换 ===
    void onFind();
    void onReplace();

    // === 状态栏更新 ===
    void updateStatusBar();

    // === 窗口标题联动（文档修改信号）===
    void onDocumentModified(bool modified);

    // === 粘贴按钮使能（剪贴板变化）===
    void onClipboardChanged();

private:
    // 从文件读取到 textEdit
    bool loadFile(const QString &path);
    // 将 textEdit 写入文件
    bool saveFile(const QString &path);
    // 更新窗口标题（根据当前文件名 + 修改状态）
    void updateWindowTitle();

private:
    Ui::MainWindow *ui;

    // 当前文件路径（为空表示尚未保存过的新文件）
    QString m_currentFilePath;
};

#endif // MAINWINDOW_H
