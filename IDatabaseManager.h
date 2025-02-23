#ifndef IDATABASEMANAGER_H
#define IDATABASEMANAGER_H

#include <QObject>
#include <QVariant>
#include <QVariantMap>
#include <QVector>

// 错误信息结构体
struct DatabaseError {
    int code = 0;
    QString message;
    QString details;
};

// 抽象数据库接口类
class IDatabaseInterface {
public:
    virtual ~IDatabaseInterface() = default;

    //--------------- 连接管理 ---------------
    virtual bool open(const QVariantMap &connectionParams) = 0;
    virtual void close() = 0;
    virtual bool isOpen() const = 0;

    //--------------- 基础CRUD操作 ---------------
    virtual QVariant create(const QString &table, const QVariantMap &data) = 0;
    virtual bool update(const QString &table, const QVariantMap &data, const QVariantMap &where) = 0;
    virtual bool remove(const QString &table, const QVariantMap &where) = 0;
    virtual QVector<QVariantMap> read(const QString &table, const QVariantMap &where = {}, const QStringList &fields = {}) = 0;

    //--------------- 批量操作 ---------------
    virtual bool batchCreate(const QString &table, const QVector<QVariantMap> &dataList) {
        // 默认实现（可被子类覆盖）
        for (const auto &data : dataList) {
            if (create(table, data).isNull()) return false;
        }
        return true;
    }

    //--------------- 事务支持 ---------------
    virtual bool beginTransaction() { return false; } // 默认不支持
    virtual bool commit() { return false; }
    virtual bool rollback() { return false; }

    //--------------- 错误处理 ---------------
    virtual DatabaseError lastError() const = 0;
    virtual void clearError() = 0;

    //--------------- 扩展钩子 ---------------
    virtual QVariant executeRaw(const QString &command, const QVariant &params = QVariant()) {
        // 用于执行数据库特定命令（如SQL语句、MongoDB的JSON命令）
        Q_UNUSED(command);
        Q_UNUSED(params);
        return QVariant();
    }
};

#endif // IDATABASEMANAGER_H
