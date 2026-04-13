#ifndef TCPCONNECTIONPOOL_H
#define TCPCONNECTIONPOOL_H

#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QSemaphore>
#include <QVector>
#include <QThread>
#include <QHostAddress>
#include <QTcpSocket>
#include <QMap>
#include <QElapsedTimer>
#include <QTimer>
#include <tcp/connectionworker.h>

class TCPConnectionPool : public QObject
{
    Q_OBJECT

public:
    static TCPConnectionPool* getInstance();
    ~TCPConnectionPool();

    // 初始化连接池
    bool initialize(const QString &serverHost,
                    int basePort,
                    int maxConnections = 20,
                    int connectionTimeout = 5000);

    // 获取一个可用连接
    ConnectionWorker* getConnection(int timeoutMs = 5000);

    // 释放连接回池
    void releaseConnection(ConnectionWorker* connection);

    // 关闭连接池
    void shutdown();

    // 获取可用连接数量
    int availableConnectionCount();

    // 获取总连接数量
    int totalConnectionCount();

    // 检查所有连接的健康状态
    void checkConnections();
    
    // 打印连接池状态
    void printPoolStatus();

signals:
    void connectionError(const QString &errorMessage);
    void poolStateChanged(int active, int available, int total);

private:
    explicit TCPConnectionPool(QObject *parent = nullptr);

    // 创建新连接
    ConnectionWorker* createConnection();

    // 检查连接有效性
    bool isConnectionValid(ConnectionWorker* connection);

    // 重置所有连接
    void resetAllConnections();

    // 日志记录
    void logMessage(const QString &message, bool isError = false);

private slots:
    // 定时检查连接池状态
    void onCheckTimer();

    // 处理连接错误
    void onConnectionError(ConnectionWorker* connection, const QString &error);

private:
    static TCPConnectionPool* instance;
    static QMutex instanceMutex;

    QQueue<ConnectionWorker*> m_availableConnections;
    QVector<ConnectionWorker*> m_activeConnections;
    QVector<ConnectionWorker*> m_allConnections;
    QMutex m_poolMutex;
    QSemaphore m_connectionSemaphore;
    QTimer* m_checkTimer;

    QString m_serverHost;
    int m_basePort;
    int m_maxConnections;
    int m_connectionTimeout;
    bool m_initialized;
    bool m_shuttingDown;

    // 连接端口管理
    QVector<int> m_usedPorts;
    int m_currentPortIndex;

    // 统计信息
    int m_totalRequests;
    int m_failedRequests;
    int m_reconnections;
    QElapsedTimer m_uptime;
};

#endif // TCPCONNECTIONPOOL_H
