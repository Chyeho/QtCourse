#include "patientwidget.h"
#include "database.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>
#include <QHeaderView>
#include <QSqlTableModel>
#include <QMessageBox>
#include <QSqlError>

PatientWidget::PatientWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    initModel();
}

void PatientWidget::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // 工具栏：搜索框 + 查找 / 添加 / 删除 / 修改
    auto *toolLayout = new QHBoxLayout();
    toolLayout->setSpacing(10);

    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setFixedWidth(220);
    m_searchEdit->setPlaceholderText("输入编号 / 姓名 / 身份证 / 手机号");
    m_searchBtn = new QPushButton("查找", this);
    m_addBtn = new QPushButton("添加", this);
    m_deleteBtn = new QPushButton("删除", this);
    m_editBtn = new QPushButton("修改", this);

    toolLayout->addWidget(m_searchEdit);
    toolLayout->addWidget(m_searchBtn);
    toolLayout->addWidget(m_addBtn);
    toolLayout->addWidget(m_deleteBtn);
    toolLayout->addWidget(m_editBtn);
    toolLayout->addStretch();

    mainLayout->addLayout(toolLayout);

    // 患者列表
    m_tableView = new QTableView(this);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    mainLayout->addWidget(m_tableView);

    // 信号槽
    connect(m_searchBtn, &QPushButton::clicked, this, &PatientWidget::doSearch);
    connect(m_searchEdit, &QLineEdit::returnPressed, this, &PatientWidget::doSearch);
    connect(m_deleteBtn, &QPushButton::clicked, this, &PatientWidget::doDelete);
}

void PatientWidget::initModel()
{
    if (!Database::instance().isOpen())
        return;

    m_model = new QSqlTableModel(this, Database::instance().db());
    m_model->setTable("patient");
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    // 表头中文化
    m_model->setHeaderData(0, Qt::Horizontal, "编号");
    m_model->setHeaderData(1, Qt::Horizontal, "身份证");
    m_model->setHeaderData(2, Qt::Horizontal, "姓名");
    m_model->setHeaderData(3, Qt::Horizontal, "性别");
    m_model->setHeaderData(4, Qt::Horizontal, "出生日期");
    m_model->setHeaderData(5, Qt::Horizontal, "身高(cm)");
    m_model->setHeaderData(6, Qt::Horizontal, "体重(kg)");
    m_model->setHeaderData(7, Qt::Horizontal, "手机号");
    m_model->setHeaderData(8, Qt::Horizontal, "年龄");
    m_model->setHeaderData(9, Qt::Horizontal, "创建时间");

    m_tableView->setModel(m_model);
    refresh();
}

void PatientWidget::refresh()
{
    if (m_model)
        m_model->select();
}

void PatientWidget::doSearch()
{
    if (!m_model)
        return;

    const QString kw = m_searchEdit->text().trimmed();
    if (kw.isEmpty()) {
        m_model->setFilter("");
    } else {
        const QString esc = QString(kw).replace(QLatin1String("'"), QLatin1String("''"));
        m_model->setFilter(QString(
            "NAME LIKE '%%1%' OR MOBILEPHONE LIKE '%%1%' "
            "OR ID LIKE '%%1%' OR ID_CARD LIKE '%%1%'").arg(esc));
    }
    m_model->select();

    // 查询无结果时给出提示
    if (m_model->rowCount() == 0) {
        QMessageBox::information(this, "提示", "未查询到符合条件的患者记录");
    }
}

void PatientWidget::doDelete()
{
    if (!m_model)
        return;

    const int row = m_tableView->currentIndex().row();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先在列表中选择要删除的患者记录");
        return;
    }

    const QString name = m_model->index(row, 2).data().toString();
    const auto ret = QMessageBox::question(this, "删除确认",
        QString("确定删除患者【%1】的记录吗？").arg(name),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret != QMessageBox::Yes)
        return;

    if (!m_model->removeRow(row)) {
        QMessageBox::critical(this, "删除失败", m_model->lastError().text());
        return;
    }
    if (!m_model->submitAll()) {
        QMessageBox::critical(this, "删除失败", m_model->lastError().text());
        m_model->revertAll();
        return;
    }
    refresh();
}
