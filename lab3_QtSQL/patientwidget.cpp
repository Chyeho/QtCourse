#include "patientwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>

PatientWidget::PatientWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
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
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(m_tableView);
}
