#include <tcp/tcpconnectionpool.h>
#include <QDebug>
#include <QCoreApplication>
#include <QTimer>
#include <QDateTime>

// 初始化静态成员
TCPConnectionPool* TCPConnectionPool::instance = nullptr;
QMutex TCPConnectionPool::instanceMutex;

TCPConnectionPool* TCPConnectionPool::getInstance()
{
    if (!instance) {
        QMutexLocker locker(&instanceMutex);
        if (!instance) {
            instance = new TCPConnectionPool();
        }
    }
    return instance;
}

TCPConnectionPool::TCPConnectionPool(QObject *parent)
    : QObject(parent),
      m_connectionSemaphore(0),
      m_checkTimer(new QTimer(this)),
      m_basePort(0),
      m_maxConnections(0),
      m_connectionTimeout(5000),
      m_initialized(false),
      m_shuttingDown(false),
      m_currentPortIndex(0),
      m_totalRequests(0),
      m_failedRequests(0),
      m_reconnections(0)
{
    // 设置连接检查定时器
//    connect(m_checkTimer, &QTimer::timeout, this, &TCPConnectionPool::onCheckTimer);
//    m_checkTimer->start(30000); // 每30秒检查一次连接状态

//    m_uptime.start();

    qRegisterMetaType<ConnectionWorker*>("ConnectionWorker*");
}

TCPConnectionPool::~TCPConnectionPool()
{
    shutdown();
}

bool TCPConnectionPool::initialize(const QString &serverHost, int basePort, int maxConnections, int connectionTimeout)
{
    QMutexLocker locker(&m_poolMutex);
    if (m_initialized) {
        logMessage(QString::fromLocal8Bit("连接池已初始化，重新初始化..."));
        shutdown();
    }
    qDebug()<<QString::fromLocal8Bit("初始化连接池: 服务器=%1, 基础端口=%2, 最大连接数=%3")
            .arg(serverHost).arg(basePort).arg(maxConnections);
    m_serverHost = serverHost;
    m_basePort = basePort;
    m_maxConnections = maxConnections;
    m_connectionTimeout = connectionTimeout;
    m_shuttingDown = false;
    m_usedPorts.clear();

    // 预留端口
    for (int i = 0; i < maxConnections; ++i) {
        m_usedPorts.append(m_basePort + i);
    }

    // 预创建一部分连接
    int preCreateCount = qMin(maxConnections / 2, 5); // 预创建5个或一半连接（取较小值）
    bool allSuccess = true;

    for (int i = 0; i < preCreateCount; ++i) {
        ConnectionWorker* connection = createConnection();
        if (connection) {
            m_availableConnections.enqueue(connection);
            m_allConnections.append(connection);
            m_connectionSemaphore.release(1);
            this->printPoolStatus();
        }
        else {
            allSuccess = false;
            logMessage(QString::fromLocal8Bit("预创建连接失败"), true);
        }
    }

    m_initialized = true;
    emit poolStateChanged(m_activeConnections.size(), m_availableConnections.size(), m_allConnections.size());

    logMessage(QString::fromLocal8Bit("连接池初始化%1, 预创建了 %2 个连接")
              .arg(allSuccess ? QString::fromLocal8Bit("成功") : QString::fromLocal8Bit("部分成功"))
              .arg(m_availableConnections.size()));

    return m_availableConnections.size() > 0; // 至少有一个连接可用
}

ConnectionWorker* TCPConnectionPool::createConnection()
{
    if (m_allConnections.size() >= m_maxConnections) {
        logMessage(QString::fromLocal8Bit("已达到最大连接数，无法创建新连接"), true);
        return nullptr;
    }
    // 选择下一个可用端口
    int port = m_usedPorts.at(0);
//    m_currentPortIndex = (m_currentPortIndex + 1) % m_usedPorts.size();

    ConnectionWorker* worker = new ConnectionWorker(m_serverHost, port);

    // 开始工作线程
    worker->start();

    // 等待连接完成或超时
    QElapsedTimer timer;
    timer.start();

    while (worker->status() ==TaskStatus::UNKNOWN);//堵塞等待连接准备好

    if (worker->status() == TaskStatus::ERROR) {
        logMessage(QString::fromLocal8Bit("创建连接失败: %1:%2").arg(m_serverHost).arg(port), true);
        worker->deleteLater();
        return nullptr;
    }

    logMessage(QString::fromLocal8Bit("成功创建新连接到 %1:%2").arg(m_serverHost).arg(port));
    return worker;
}

ConnectionWorker* TCPConnectionPool::getConnection(int timeoutMs)
{
    if (!m_initialized) {
        emit connectionError(QString::fromLocal8Bit("连接池未初始化"));
        m_failedRequests++;
        return nullptr;
    }

    if (m_shuttingDown) {
        emit connectionError(QString::fromLocal8Bit("连接池正在关闭"));
        m_failedRequests++;
        return nullptr;
    }

    m_totalRequests++;
    
    // 尝试获取一个信号量，最多等待指定的超时时间
    if (!m_connectionSemaphore.tryAcquire(1, timeoutMs)){
        QMutexLocker locker(&m_poolMutex);
        
        // 创建新连接（如果未达到最大连接数）
        if (m_allConnections.size() < m_maxConnections) {
            ConnectionWorker* newConnection = createConnection();
            if (newConnection) {
                m_activeConnections.append(newConnection);
                m_allConnections.append(newConnection);
                emit poolStateChanged(m_activeConnections.size(), m_availableConnections.size(), m_allConnections.size());
                return newConnection;
            }
        }

        logMessage(QString::fromLocal8Bit("获取连接超时，没有可用连接"), true);
        m_failedRequests++;
        emit connectionError(QString::fromLocal8Bit("获取连接超时，请稍后重试"));
        return nullptr;
    }

    // 从可用队列获取一个连接
//    QMutexLocker locker(&m_poolMutex);
//    if (m_availableConnections.isEmpty()) {
//        // 这种情况理论上不应该发生，因为信号量控制着可用连接的数量
//        m_connectionSemaphore.release(); // 释放刚获取的信号量
//        logMessage(QString::fromLocal8Bit("信号量与可用连接队列不同步"), true);
//        m_failedRequests++;
//        emit connectionError(QString::fromLocal8Bit("内部错误：连接池状态异常"));
//        return nullptr;
//    }

    ConnectionWorker* connection = m_availableConnections.dequeue();
    m_activeConnections.append(connection);
    this->printPoolStatus();

//    // 检查连接有效性
//    if (!isConnectionValid(connection)) {
//        logMessage(QString::fromLocal8Bit("获取到无效连接，尝试重置"), true);

//        // 重置连接
//        if (!connection->resetConnection()) {
//            // 重置失败，从所有连接中移除，并尝试创建新连接
//            m_activeConnections.removeOne(connection);
//            m_allConnections.removeOne(connection);

//            connection->deleteLater();

//            ConnectionWorker* newConnection = createConnection();
//            if (newConnection) {
//                m_activeConnections.append(newConnection);
//                m_allConnections.append(newConnection);
//                connection = newConnection;
//                m_reconnections++;
//            }
//            else{
//                m_connectionSemaphore.release(); // 释放信号量
//                m_failedRequests++;
//                emit connectionError(QString::fromLocal8Bit("无法重新建立连接"));
//                emit poolStateChanged(m_activeConnections.size(), m_availableConnections.size(), m_allConnections.size());
//                return nullptr;
//            }
//        }
//        else {
//            m_reconnections++;
//        }
//    }

//    emit poolStateChanged(m_activeConnections.size(), m_availableConnections.size(), m_allConnections.size());
    return connection;
}

void TCPConnectionPool::releaseConnection(ConnectionWorker* connection)
{
    if (!connection) {
        return;
    }
    QMutexLocker locker(&m_poolMutex);
    // if (m_shuttingDown) {
    //     // 如果连接池正在关闭，直接清理连接
    //     if (m_allConnections.contains(connection)) {
    //         m_allConnections.removeOne(connection);
    //         m_activeConnections.removeOne(connection);
    //         connection->quit();
    //         connection->wait();
    //         connection->deleteLater();
    //     }
    //     emit poolStateChanged(m_activeConnections.size(), m_availableConnections.size(), m_allConnections.size());
    //     return;
    // }

    // 从活动连接列表中移除
    bool if_remove = m_activeConnections.removeOne(connection);

    // 检查连接状态
    if (connection->status() == TaskStatus::ERROR) {
        logMessage(QString::fromLocal8Bit("释放故障连接,获取新连接"), true);

        // 如果未达到最大连接数，创建新连接
        ConnectionWorker* newConnection = createConnection();
        if (newConnection) {
            m_availableConnections.enqueue(newConnection);
            m_allConnections.append(newConnection);
            m_connectionSemaphore.release(1); // 增加信号量
            m_reconnections++;
        }
        else{
            qDebug()<<QString::fromLocal8Bit("创建新连接失败,程序崩溃");
        }
    } 
    else {
        // 连接正常，添加回可用队列
        m_availableConnections.enqueue(connection);
        m_connectionSemaphore.release(1);
    }
    //this->printPoolStatus();
    // emit poolStateChanged(m_activeConnections.size(), m_availableConnections.size(), m_allConnections.size());
}

void TCPConnectionPool::shutdown()
{
    QMutexLocker locker(&m_poolMutex);

    if (!m_initialized || m_shuttingDown) {
        return;
    }

    logMessage(QString::fromLocal8Bit("正在关闭连接池..."));

    m_shuttingDown = true;
    m_checkTimer->stop();

    // 清空可用连接队列
    m_availableConnections.clear();

    // 关闭和删除所有连接
    for (ConnectionWorker* connection : m_allConnections) {
        connection->quit();
        connection->wait(1000);
        delete connection;
    }

    m_allConnections.clear();
    m_activeConnections.clear();
    m_usedPorts.clear();

    m_initialized = false;
    m_shuttingDown = false;

    m_connectionSemaphore.acquire(m_connectionSemaphore.available()); // 清空信号量

    logMessage(QString::fromLocal8Bit("连接池已关闭"));
    emit poolStateChanged(0, 0, 0);
}

int TCPConnectionPool::availableConnectionCount()
{
    QMutexLocker locker(&m_poolMutex);
    return m_availableConnections.size();
}

int TCPConnectionPool::totalConnectionCount()
{
    QMutexLocker locker(&m_poolMutex);
    return m_allConnections.size();
}

void TCPConnectionPool::checkConnections()
{
    QMutexLocker locker(&m_poolMutex);

    if (!m_initialized || m_shuttingDown) {
        return;
    }

    logMessage(QString::fromLocal8Bit("开始检查连接状态..."));

    QVector<ConnectionWorker*> invalidConnections;

    // 检查所有可用连接
    for (int i = 0; i < m_availableConnections.size(); ++i) {
        ConnectionWorker* connection = m_availableConnections.at(i);

        if (!isConnectionValid(connection)) {
            invalidConnections.append(connection);
        }
    }

    // 处理无效连接
    for (ConnectionWorker* connection : invalidConnections) {
        m_availableConnections.removeOne(connection);

        // 尝试重置连接
        if (!connection->resetConnection()) {
            m_allConnections.removeOne(connection);
            connection->quit();
            connection->wait();
            connection->deleteLater();

            // 创建新连接
            ConnectionWorker* newConnection = createConnection();
            if (newConnection) {
                m_availableConnections.enqueue(newConnection);
                m_allConnections.append(newConnection);
                m_reconnections++;
            } else {
                m_connectionSemaphore.acquire(); // 减少信号量
            }
        } else {
            // 重置成功，添加回可用队列
            m_availableConnections.enqueue(connection);
            m_reconnections++;
        }
    }

    logMessage(QString::fromLocal8Bit("连接检查完成，修复了 %1 个无效连接").arg(invalidConnections.size()));
    emit poolStateChanged(m_activeConnections.size(), m_availableConnections.size(), m_allConnections.size());
}

bool TCPConnectionPool::isConnectionValid(ConnectionWorker* connection)
{
    if (!connection) {
        return false;
    }

    TaskStatus status = connection->status();
    return (status != TaskStatus::ERROR && status != TaskStatus::UNKNOWN);
}

void TCPConnectionPool::resetAllConnections()
{
    QMutexLocker locker(&m_poolMutex);

    if (!m_initialized || m_shuttingDown) {
        return;
    }

    logMessage(QString::fromLocal8Bit("重置所有连接..."));

    // 收集所有连接
    QVector<ConnectionWorker*> allConnections = m_allConnections;

    // 清空连接队列
    m_availableConnections.clear();
    m_activeConnections.clear();
    m_allConnections.clear();

    // 重置信号量
    m_connectionSemaphore.acquire(m_connectionSemaphore.available());

    // 尝试重置每个连接
    int successCount = 0;
    for (ConnectionWorker* connection : allConnections) {
        if (connection->resetConnection()) {
            m_availableConnections.enqueue(connection);
            m_allConnections.append(connection);
            m_connectionSemaphore.release();
            successCount++;
        } else {
            connection->quit();
            connection->wait();
            connection->deleteLater();
        }
    }

    // 确保有足够的连接
    while (m_allConnections.size() < m_maxConnections / 2) {
        ConnectionWorker* newConnection = createConnection();
        if (newConnection) {
            m_availableConnections.enqueue(newConnection);
            m_allConnections.append(newConnection);
            m_connectionSemaphore.release();
            successCount++;
        } else {
            break;
        }
    }

    logMessage(QString::fromLocal8Bit("连接重置完成，成功: %1，失败: %2")
              .arg(successCount).arg(allConnections.size() - successCount));
    emit poolStateChanged(m_activeConnections.size(), m_availableConnections.size(), m_allConnections.size());
}

void TCPConnectionPool::onCheckTimer()
{
    // 定期检查连接状态
    checkConnections();

    // 记录一些统计信息
    int uptimeHours = m_uptime.elapsed() / (1000 * 60 * 60);
    logMessage(QString::fromLocal8Bit("连接池状态 - 运行时间: %1小时, 总请求: %2, 成功率: %3%, 重连: %4")
              .arg(uptimeHours)
              .arg(m_totalRequests)
              .arg(m_totalRequests > 0 ? 100.0 * (m_totalRequests - m_failedRequests) / m_totalRequests : 100.0)
              .arg(m_reconnections));
}

void TCPConnectionPool::onConnectionError(ConnectionWorker* connection, const QString &error)
{
    QMutexLocker locker(&m_poolMutex);

    logMessage(QString::fromLocal8Bit("连接错误: %1").arg(error), true);

    // 检查该连接是否是活动连接
    if (m_activeConnections.contains(connection)) {
        // 活动连接出错处理在 releaseConnection 中进行
        emit connectionError(error);
    } else if (m_availableConnections.contains(connection)) {
        // 可用连接出错，标记为无效连接
        m_availableConnections.removeOne(connection);

        // 尝试重置连接
        if (!connection->resetConnection()) {
            // 重置失败，销毁连接
            m_allConnections.removeOne(connection);
            connection->quit();
            connection->wait();
            connection->deleteLater();

            // 减少信号量
            m_connectionSemaphore.acquire();

            // 如果允许，创建新连接
            if (m_allConnections.size() < m_maxConnections) {
                ConnectionWorker* newConnection = createConnection();
                if (newConnection) {
                    m_availableConnections.enqueue(newConnection);
                    m_allConnections.append(newConnection);
                    m_connectionSemaphore.release();
                    m_reconnections++;
                }
            }
        } else {
            // 重置成功，重新加入队列
            m_availableConnections.enqueue(connection);
            m_reconnections++;
        }

        emit poolStateChanged(m_activeConnections.size(), m_availableConnections.size(), m_allConnections.size());
    }
}

void TCPConnectionPool::logMessage(const QString &message, bool isError)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString logMsg = QString("[%1] [ConnectionPool] %2").arg(timestamp).arg(message);

    if (isError) {
        qWarning() << logMsg;
    } else {
        qDebug() << logMsg;
    }
}

void TCPConnectionPool::printPoolStatus()
{
    
    qDebug() << QString::fromLocal8Bit("=================== TCP 连接池状态 ===================");
    qDebug() << QString::fromLocal8Bit("信号量计数:       %1").arg(m_connectionSemaphore.available());
    qDebug() << QString::fromLocal8Bit("总连接数:         %1").arg(m_allConnections.size());
    qDebug() << QString::fromLocal8Bit("可用连接数:       %1").arg(m_availableConnections.size());
    qDebug() << QString::fromLocal8Bit("活跃连接数:       %1").arg(m_activeConnections.size());
    
    // 打印每个连接的状态详情
    qDebug() << QString::fromLocal8Bit("连接状态详情:");
    for (int i = 0; i < m_allConnections.size(); i++) {
        ConnectionWorker* conn = m_allConnections[i];
        QString status;
        
        if (m_availableConnections.contains(conn)) {
            status = QString::fromLocal8Bit("可用");
        } else if (m_activeConnections.contains(conn)) {
            status = QString::fromLocal8Bit("活跃");
        } else {
            status = QString::fromLocal8Bit("未知");
        }
        
        qDebug() << QString::fromLocal8Bit("  连接 %1: %2 (状态: %3)")
                    .arg(i + 1)
                    .arg(conn->hostPort())
                    .arg(status);
    }
    qDebug() << "====================================================";
}
