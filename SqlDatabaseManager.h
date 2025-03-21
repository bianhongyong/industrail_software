#pragma once

#include <QSqlDatabase>
#include <QVariantMap>
#include <QVector>
#include <QStringList>
#include <IDatabaseManager.h>
#include <QObject>
#include <QThread>
#include <QTimer>
#include <QElapsedTimer>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QSqlRecord>
#include <QDateTime>
class SQLDatabase : public IDatabaseInterface {
    Q_OBJECT
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
    QString connName;
    void setError(const QSqlError &error);
    void setError(int code, const QString &message);
    bool executeQuery(QSqlQuery &query);
    QString generateConnectionName() const;

signals:
    void close_sql();
};
// 子线程检测类
class DbChecker : public QThread {
    Q_OBJECT
private:
    QString connName;
    QSqlDatabase db;
public:
    void run() override {
        connName = QString("check_%1").arg((quintptr)currentThreadId());
        db = QSqlDatabase::addDatabase("QODBC", connName);
        db.setHostName(QString("211.81.50.204"));
        db.setDatabaseName(QString("mould"));
        db.setUserName(QString("bhy"));
        db.setPassword(QString("bhybhy123456"));



        if (!db.open()) {
            emit pingResult(false);  // 发送连接失败信号[6](@ref)
            return;  // 终止线程运行
        }

        while (!isInterruptionRequested()) {
            bool alive = false;
            QSqlQuery query(db);
            if (query.exec("SELECT 1")) {
                alive = true;
            }
            emit pingResult(alive);  // 发送检测结果信号
            QThread::sleep(3);       // 间隔5秒检测一次
        }

        if(db.isOpen()){
            db.close();
            QSqlDatabase::removeDatabase(connName);
        }
        qDebug()<<"over";
    }


    ~DbChecker(){

    }
signals:
    void pingResult(bool isAlive);
};


