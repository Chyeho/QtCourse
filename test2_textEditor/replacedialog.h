#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H

#include <QDialog>

namespace Ui {
class ReplaceDialog;
}

class ReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReplaceDialog(QWidget *parent = nullptr);
    ~ReplaceDialog();

    // 暴露 UI 供外部访问（MainWindow 需要读取控件状态）
    Ui::ReplaceDialog *ui;
};

#endif // REPLACEDIALOG_H
