#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include "SqlDatabaseManager.h"
#include <QSqlRecord>
SQLDatabase::SQLDatabase(const QString &driver):m_db(QSqlDatabase::addDatabase(driver,"connection")),m_driver(driver)  {

}

SQLDatabase::~SQLDatabase() {
    emit close_sql();
    close();
}

// 连接管理
bool SQLDatabase::open(const QVariantMap &params) {
    if (m_db.isOpen()){
        close();
      }
    if (params.contains("database")) m_db.setDatabaseName(params["database"].toString());
    if (params.contains("host")) m_db.setHostName(params["host"].toString());
    if (params.contains("user")) m_db.setUserName(params["user"].toString());
    if (params.contains("password")) m_db.setPassword(params["password"].toString());

    if (!m_db.open()) {

        setError(m_db.lastError());
        return false;
    }
    return true;
}

void SQLDatabase::close() {
    if (m_db.isOpen()) {
        m_db.close();
    }
    QSqlDatabase::removeDatabase(m_db.connectionName());

}

bool SQLDatabase::isOpen() const {
    return m_db.isOpen();
}

// CRUD 操作
QVariant SQLDatabase::create(const QString &table, const QVariantMap &data) {
    if (data.isEmpty()) {
        setError(-1, "Empty data for creation");
        return QVariant();
    }

    QStringList fields = data.keys();
    QString placeholders = ":" + fields.join(", :");
    QString sql = QString("INSERT INTO %1 (%2) VALUES (%3)")
                      .arg(table, fields.join(", "), placeholders);

    QSqlQuery query(m_db);
    if (!query.prepare(sql)) {
        setError(query.lastError());
        return QVariant();
    }

    for (const QString &key : fields) {
        query.bindValue(":" + key, data[key]);
    }

    if (query.exec()) {
        return query.lastInsertId();
    } else {
        setError(query.lastError());
        return QVariant();
    }
}

bool SQLDatabase::update(const QString &table, const QVariantMap &data, const QVariantMap &where) {
    if (data.isEmpty() || where.isEmpty()) {
        setError(-1, "Invalid data or where clause");
        return false;
    }

    QStringList setClauses;
    for (auto it = data.begin(); it != data.end(); ++it) {
        setClauses << QString("%1 = :set_%2").arg(it.key()).arg(it.key());
    }

    QStringList whereClauses;
    for (auto it = where.begin(); it != where.end(); ++it) {
        whereClauses << QString("%1 = :where_%2").arg(it.key()).arg(it.key());
    }

    QString sql = QString("UPDATE %1 SET %2 WHERE %3")
                      .arg(table)
                      .arg(setClauses.join(", "))
                      .arg(whereClauses.join(" AND "));

    QSqlQuery query(m_db);
    if (!query.prepare(sql)) {
        setError(query.lastError());
        return false;
    }

    for (auto it = data.begin(); it != data.end(); ++it) {
        query.bindValue(":set_" + it.key(), it.value());
    }

    for (auto it = where.begin(); it != where.end(); ++it) {
        query.bindValue(":where_" + it.key(), it.value());
    }

    return executeQuery(query);
}

bool SQLDatabase::remove(const QString &table, const QVariantMap &where) {
    if (where.isEmpty()) {
        setError(-1, "Empty where clause is not allowed for delete");
        return false;
    }

    QStringList whereClauses;
    for (auto it = where.begin(); it != where.end(); ++it) {
        whereClauses << QString("%1 = :%2").arg(it.key()).arg(it.key());
    }

    QString sql = QString("DELETE FROM %1 WHERE %2")
                      .arg(table)
                      .arg(whereClauses.join(" AND "));

    QSqlQuery query(m_db);
    if (!query.prepare(sql)) {
        setError(query.lastError());
        return false;
    }

    for (auto it = where.begin(); it != where.end(); ++it) {
        query.bindValue(":" + it.key(), it.value());
    }

    return executeQuery(query);
}

QVector<QVariantMap> SQLDatabase::read(const QString &table, const QVariantMap &where, const QStringList &fields) {
    QStringList fieldList = fields.isEmpty() ? QStringList("*") : fields;
    QString sql = QString("SELECT %1 FROM %2").arg(fieldList.join(", ")).arg(table);

    QVector<QVariantMap> results;
    QSqlQuery query(m_db);

    if (!where.isEmpty()) {
        QStringList whereClauses;
        for (auto it = where.begin(); it != where.end(); ++it) {
            whereClauses << QString("%1 = :%2").arg(it.key()).arg(it.key());
        }
        sql += " WHERE " + whereClauses.join(" AND ");
    }

    if (!query.prepare(sql)) {
        setError(query.lastError());
        return results;
    }

    for (auto it = where.begin(); it != where.end(); ++it) {
        query.bindValue(":" + it.key(), it.value());
    }

    if (!query.exec()) {
        setError(query.lastError());
        return results;
    }

    while (query.next()) {
        QVariantMap row;
        for (int i = 0; i < query.record().count(); ++i) {
            row[query.record().fieldName(i)] = query.value(i);
        }
        results.append(row);
    }

    return results;
}

// 事务支持
bool SQLDatabase::beginTransaction() {
    return m_db.transaction();
}

bool SQLDatabase::commit() {
    return m_db.commit();
}

bool SQLDatabase::rollback() {
    return m_db.rollback();
}

// 错误处理
DatabaseError SQLDatabase::lastError() const {
    return m_lastError;
}

void SQLDatabase::clearError() {
    m_lastError = DatabaseError{};
}

// 扩展功能
QVariant SQLDatabase::executeRaw(const QString &command, const QVariant &params) {
    QSqlQuery query(m_db);
    if (query.exec(command)) {
        if (query.isSelect()) {
            QVector<QVariantMap> results;
            while (query.next()) {
                QVariantMap row;
                for (int i = 0; i < query.record().count(); ++i) {
                    row[query.record().fieldName(i)] = query.value(i);
                }
                results.append(row);
            }
            return QVariant::fromValue(results);
        } else {
            return query.numRowsAffected();
        }
    } else {
        setError(query.lastError());
        return QVariant();
    }
}

QSqlDatabase SQLDatabase::database()
{
    return m_db;
}

void SQLDatabase::setError(const QSqlError &error) {
    m_lastError = { error.number(), error.text(), error.databaseText() };
}

void SQLDatabase::setError(int code, const QString &message) {
    m_lastError = { code, message, "" };
}

bool SQLDatabase::executeQuery(QSqlQuery &query) {
    if (!query.exec()) {
        setError(query.lastError());
        return false;
    }
    return true;
}

QString SQLDatabase::generateConnectionName() const {
    static int counter = 0;
    return QString("SQLConnection_%1_%2").arg(m_driver).arg(++counter);
}
