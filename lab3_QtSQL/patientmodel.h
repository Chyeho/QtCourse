#ifndef PATIENTMODEL_H
#define PATIENTMODEL_H

#include <QSqlTableModel>

// 患者表模型：在 QSqlTableModel 基础上做显示层映射
// 性别列 SEX：数据库存 1/0，界面显示「男 / 女」
class PatientModel : public QSqlTableModel
{
    Q_OBJECT
public:
    using QSqlTableModel::QSqlTableModel;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

#endif // PATIENTMODEL_H
