#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>

namespace Ui {
class FindDialog;
}

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(QWidget *parent = nullptr);
    ~FindDialog();

    // 暴露 UI 供外部访问（MainWindow 需要读取控件状态）
    Ui::FindDialog *ui;
};

#endif // FINDDIALOG_H
