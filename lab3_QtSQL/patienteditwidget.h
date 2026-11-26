#ifndef PATIENTEDITWIDGET_H
#define PATIENTEDITWIDGET_H

#include <QWidget>

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

private:
    void setupUi();

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
