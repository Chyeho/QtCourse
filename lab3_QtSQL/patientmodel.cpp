#include "patientmodel.h"

// 性别列在 patient 表中的位置
static constexpr int SexColumn = 3;

QVariant PatientModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole && index.column() == SexColumn) {
        const int sex = QSqlTableModel::data(index, Qt::DisplayRole).toInt();
        return sex == 1 ? QStringLiteral("男") : QStringLiteral("女");
    }
    return QSqlTableModel::data(index, role);
}
