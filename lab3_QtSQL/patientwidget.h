#ifndef PATIENTWIDGET_H
#define PATIENTWIDGET_H

#include <QWidget>

class QLineEdit;
class QPushButton;
class QTableView;
class QSqlTableModel;

// 患者管理页面（对应实验参考图 3）：查询框 + 操作按钮 + 患者列表
class PatientWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PatientWidget(QWidget *parent = nullptr);

    QLineEdit *searchEdit() const { return m_searchEdit; }
    QPushButton *searchButton() const { return m_searchBtn; }
    QPushButton *addButton() const { return m_addBtn; }
    QPushButton *deleteButton() const { return m_deleteBtn; }
    QPushButton *editButton() const { return m_editBtn; }
    QTableView *tableView() const { return m_tableView; }
    QSqlTableModel *model() const { return m_model; }

    // 增删改后重新从数据库加载并刷新列表
    void refresh();

private:
    void setupUi();
    void initModel();
    void doSearch();
    void doDelete();

    QLineEdit *m_searchEdit = nullptr;
    QPushButton *m_searchBtn = nullptr;
    QPushButton *m_addBtn = nullptr;
    QPushButton *m_deleteBtn = nullptr;
    QPushButton *m_editBtn = nullptr;
    QTableView *m_tableView = nullptr;
    QSqlTableModel *m_model = nullptr;
};

#endif // PATIENTWIDGET_H
