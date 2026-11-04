#include "finddialog.h"
#include "ui_finddialog.h"

FindDialog::FindDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FindDialog)
{
    ui->setupUi(this);

    // 方向默认：向下
    ui->radioDown->setChecked(true);

    // 打开对话框时直接把焦点放到输入框，方便直接输入
    ui->findEdit->setFocus();

    // 按钮行为：查找下一个 → Accepted；取消 → Rejected
    ui->btnFindNext->setDefault(true);
    connect(ui->btnFindNext, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->btnCancel,   &QPushButton::clicked, this, &QDialog::reject);
}

FindDialog::~FindDialog()
{
    delete ui;
}
