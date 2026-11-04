#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>
#include <QTextDocument>

namespace Ui {
class FindDialog;
}

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(QWidget *parent = nullptr);
    ~FindDialog();

    // 暴露 UI 供外部访问
    Ui::FindDialog *ui;

signals:
    // 点击"查找下一个"时发出；对话框本身不关闭，便于连续查找
    void searchRequested(const QString &text, QTextDocument::FindFlags flags);

private:
    // 读取当前对话框里的方向/大小写选项
    QTextDocument::FindFlags findFlags() const;
};

#endif // FINDDIALOG_H
