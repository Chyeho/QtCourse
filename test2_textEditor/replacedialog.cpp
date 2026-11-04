#include "replacedialog.h"
#include "ui_replacedialog.h"

ReplaceDialog::ReplaceDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ReplaceDialog)
{
    ui->setupUi(this);

    // 方向默认：向下
    ui->radioDown->setChecked(true);

    ui->findEdit->setFocus();

    // 三个功能按钮只发信号，不关闭对话框，便于连续操作
    connect(ui->btnFindNext, &QPushButton::clicked, this, [this]() {
        const QString text = ui->findEdit->text();
        if (text.isEmpty()) {
            return;
        }
        emit searchRequested(text, findFlags());
    });

    connect(ui->btnReplace, &QPushButton::clicked, this, [this]() {
        const QString text = ui->findEdit->text();
        if (text.isEmpty()) {
            return;
        }
        emit replaceRequested(text, ui->replaceEdit->text(), findFlags());
    });

    connect(ui->btnReplaceAll, &QPushButton::clicked, this, [this]() {
        const QString text = ui->findEdit->text();
        if (text.isEmpty()) {
            return;
        }
        emit replaceAllRequested(text, ui->replaceEdit->text(), findFlags());
    });

    // 取消：关闭（隐藏）对话框
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

ReplaceDialog::~ReplaceDialog()
{
    delete ui;
}

QTextDocument::FindFlags ReplaceDialog::findFlags() const
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
