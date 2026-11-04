#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <QPixmap>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    // 窗口标题
    setWindowTitle(QStringLiteral("关于"));

    // OK 按钮：点击后关闭对话框（Accepted）
    connect(ui->okBtn, &QPushButton::clicked, this, &QDialog::accept);

    // 加载 qrc 里已注册的 about.png 图标
    // 注意：qrc prefix 是 /icon，file 里写的是 icon/about.png，
    // 所以完整资源路径是 :/icon/icon/about.png（两个 icon）
    QPixmap pix(":/icon/icon/about.png");
    if (!pix.isNull()) {
        ui->label_7->setPixmap(pix);
        // 按 label 尺寸缩放保持比例
        ui->label_7->setScaledContents(true);
    }
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
