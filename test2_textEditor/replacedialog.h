#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H

#include <QDialog>
#include <QTextDocument>

namespace Ui {
class ReplaceDialog;
}

class ReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReplaceDialog(QWidget *parent = nullptr);
    ~ReplaceDialog();

    // 暴露 UI 供外部访问
    Ui::ReplaceDialog *ui;

signals:
    // 点击"查找下一个"时发出（对话框保持打开）
    void searchRequested(const QString &text, QTextDocument::FindFlags flags);
    // 点击"替换"时发出（替换一处并继续查找下一个）
    void replaceRequested(const QString &text, const QString &replacement,
                          QTextDocument::FindFlags flags);
    // 点击"全部替换"时发出
    void replaceAllRequested(const QString &text, const QString &replacement,
                             QTextDocument::FindFlags flags);

private:
    // 读取当前对话框里的方向/大小写选项
    QTextDocument::FindFlags findFlags() const;
};

#endif // REPLACEDIALOG_H
