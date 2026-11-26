#include "database.h"
#include <QSqlError>

Database &Database::instance()
{
    static Database inst;
    return inst;
}

bool Database::open()
{
    if (m_opened && m_db.isOpen())
        return true;

    // 驱动是否可用（qsqlmysql.dll）
    if (!QSqlDatabase::isDriverAvailable("QMYSQL")) {
        m_lastError = "未加载 MySQL 驱动（QMYSQL），请检查 sqldrivers 插件与 libmysql.dll";
        m_opened = false;
        return false;
    }

    m_db = QSqlDatabase::addDatabase("QMYSQL");
    m_db.setHostName("localhost");
    m_db.setPort(3306);
    m_db.setDatabaseName("hospital_system");
    m_db.setUserName("root");
    m_db.setPassword("chengdihong666");

    if (!m_db.open()) {
        m_lastError = m_db.lastError().text();
        m_opened = false;
        return false;
    }

    m_opened = true;
    m_lastError.clear();
    return true;
}

void Database::close()
{
    if (m_db.isOpen())
        m_db.close();
    m_opened = false;
}

bool Database::isOpen() const
{
    return m_opened && m_db.isOpen();
}

QSqlDatabase Database::db() const
{
    return m_db;
}

QString Database::lastError() const
{
    return m_lastError;
}
