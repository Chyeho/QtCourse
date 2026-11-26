#include "finddialog.h"
#include "ui_finddialog.h"

FindDialog::FindDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FindDialog)
{
    ui->setupUi(this);

    // 方向默认：向下
    ui->radioDown->setChecked(true);

    // 打开对话框时直接把焦点放到输入框
    ui->findEdit->setFocus();
    ui->btnFindNext->setDefault(true);

    // 查找下一个：只发出查找请求，不关闭对话框，可连续点击
    connect(ui->btnFindNext, &QPushButton::clicked, this, [this]() {
        const QString text = ui->findEdit->text();
        if (text.isEmpty()) {
            return;
        }
        emit searchRequested(text, findFlags());
    });

    // 取消：关闭（隐藏）对话框
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

FindDialog::~FindDialog()
{
    delete ui;
}

QTextDocument::FindFlags FindDialog::findFlags() const
{
    QTextDocument::FindFlags flags;
    if (ui->radioUp->isChecked()) {
        flags |= QTextDocument::FindBackward;
    }
    if (ui->caseSensitiveCheck->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    return flags;
}
