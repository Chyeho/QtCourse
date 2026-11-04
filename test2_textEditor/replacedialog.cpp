#include "replacedialog.h"
#include "ui_replacedialog.h"

ReplaceDialog::ReplaceDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ReplaceDialog)
{
    ui->setupUi(this);

    // 方向默认：向下
    ui->radioDown->setChecked(true);

    // 按钮行为：每个按钮都 accept，但通过不同的 result 值区分
    // btnCancel → reject
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject);

    // 其他三个按钮 → 不同的 result 值
    connect(ui->btnFindNext,   &QPushButton::clicked, this, [this](){ setResult(1); accept(); });
    connect(ui->btnReplace,    &QPushButton::clicked, this, [this](){ setResult(2); accept(); });
    connect(ui->btnReplaceAll, &QPushButton::clicked, this, [this](){ setResult(3); accept(); });
}

ReplaceDialog::~ReplaceDialog()
{
    delete ui;
}
