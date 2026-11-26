#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QSqlDatabase>

// 数据库连接单例：负责 MySQL 连接的建立、状态与错误信息
class Database
{
public:
    static Database &instance();

    // 建立数据库连接，成功返回 true；失败时错误信息可通过 lastError() 获取
    bool open();
    void close();

    bool isOpen() const;
    QSqlDatabase db() const;
    QString lastError() const;

private:
    Database() = default;
    Q_DISABLE_COPY(Database)

    QSqlDatabase m_db;
    QString m_lastError;
    bool m_opened = false;
};

#endif // DATABASE_H
