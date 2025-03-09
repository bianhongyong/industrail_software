#ifndef SERIALCOMMUNICATION_H
#define SERIALCOMMUNICATION_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMap>

class SerialCommunication : public QObject
{
    Q_OBJECT

public:
    explicit SerialCommunication(QObject *parent = nullptr);
    ~SerialCommunication();

    bool openSerialPort(const QString &portName, int baudRate);
    void closeSerialPort();
    bool sendToSTM32(const QByteArray &data);
    QByteArray receiveFromSTM32();
    QMap<QString, QString> availablePorts();  // 修改这一行
    bool isOpen();
private slots:
    void handleReadyRead();

private:
    QSerialPort *serial;
signals:
    void serial_portFailopen();
    void serial_senddata(QByteArray data);
};

#endif
