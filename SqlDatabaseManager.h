#pragma once

#include <QSqlDatabase>
#include <QVariantMap>
#include <QVector>
#include <QStringList>
#include <IDatabaseManager.h>

class SQLDatabase : public IDatabaseInterface {
public:
    SQLDatabase(const QString &driver);
    ~SQLDatabase() override;

    // 连接管理
    bool open(const QVariantMap &params) override;
    void close() override;
    bool isOpen() const override;

    // CRUD 操作
    QVariant create(const QString &table, const QVariantMap &data) override;
    bool update(const QString &table, const QVariantMap &data, const QVariantMap &where) override;
    bool remove(const QString &table, const QVariantMap &where) override;
    QVector<QVariantMap> read(const QString &table, const QVariantMap &where = {}, const QStringList &fields = {}) override;

    // 事务支持
    bool beginTransaction() override;
    bool commit() override;
    bool rollback() override;

    // 错误处理
    DatabaseError lastError() const override;
    void clearError() override;

    // 扩展功能
    QVariant executeRaw(const QString &command, const QVariant &params = QVariant()) override;

    QSqlDatabase database();

private:
    QSqlDatabase m_db;
    DatabaseError m_lastError;
    QString m_driver;
    void setError(const QSqlError &error);
    void setError(int code, const QString &message);
    bool executeQuery(QSqlQuery &query);
    QString generateConnectionName() const;
};
