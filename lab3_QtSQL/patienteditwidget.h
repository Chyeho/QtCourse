#ifndef PATIENTEDITWIDGET_H
#define PATIENTEDITWIDGET_H

#include <QWidget>
#include <QSqlRecord>

class QLineEdit;
class QComboBox;
class QDateEdit;
class QDoubleSpinBox;
class QPushButton;

// 患者信息编辑页面（对应实验参考图 4）：新增 / 修改共用表单
class PatientEditWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PatientEditWidget(QWidget *parent = nullptr);

    QLineEdit *idEdit() const { return m_idEdit; }
    QLineEdit *nameEdit() const { return m_nameEdit; }
    QLineEdit *idCardEdit() const { return m_idCardEdit; }
    QComboBox *sexCombo() const { return m_sexCombo; }
    QDateEdit *dobEdit() const { return m_dobEdit; }
    QDoubleSpinBox *heightSpin() const { return m_heightSpin; }
    QDoubleSpinBox *weightSpin() const { return m_weightSpin; }
    QLineEdit *mobileEdit() const { return m_mobileEdit; }
    QPushButton *saveButton() const { return m_saveBtn; }
    QPushButton *cancelButton() const { return m_cancelBtn; }

    // 进入新增模式：清空表单，编号可编辑
    void clearForm();
    // 进入修改模式：回填已有记录，编号只读
    void loadRecord(const QSqlRecord &rec);
    // 表单校验，通过返回 true；失败时 error 带出提示信息
    bool validate(QString &error) const;
    bool isAddMode() const { return m_isAddMode; }

private:
    void setupUi();

    bool m_isAddMode = true;
    QLineEdit *m_idEdit = nullptr;
    QLineEdit *m_nameEdit = nullptr;
    QLineEdit *m_idCardEdit = nullptr;
    QComboBox *m_sexCombo = nullptr;
    QDateEdit *m_dobEdit = nullptr;
    QDoubleSpinBox *m_heightSpin = nullptr;
    QDoubleSpinBox *m_weightSpin = nullptr;
    QLineEdit *m_mobileEdit = nullptr;
    QPushButton *m_saveBtn = nullptr;
    QPushButton *m_cancelBtn = nullptr;
};

#endif // PATIENTEDITWIDGET_H
