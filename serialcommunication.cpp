#include "serialcommunication.h"
#include <QDebug>

SerialCommunication::SerialCommunication(QObject *parent) : QObject(parent), serial(new QSerialPort(this))
{
    connect(serial, &QSerialPort::readyRead, this, &SerialCommunication::handleReadyRead);
}

SerialCommunication::~SerialCommunication()
{
    closeSerialPort();
}

bool SerialCommunication::openSerialPort(const QString &portName, int baudRate)
{
    serial->setPortName(portName);
    serial->setBaudRate(baudRate);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (serial->open(QIODevice::ReadWrite)) {
        qDebug() << "Serial port opened successfully.";
        return true;
    } else {
        qDebug() << "Failed to open serial port.";
        emit serial_portFailopen();
        return false;
    }
}

void SerialCommunication::closeSerialPort()
{
    if (serial->isOpen()) {
        serial->close();
        qDebug() << "Serial port closed.";
    }
}

bool SerialCommunication::sendToSTM32(const QByteArray &data)
{
    if (serial->isOpen()) {
        serial->write(data);
        return serial->waitForBytesWritten();
    }
    return false;
}


void SerialCommunication::handleReadyRead()
{
    if (serial->bytesAvailable() > 0){
        emit serial_senddata(serial->readAll());
    }
}

QMap<QString, QString> SerialCommunication::availablePorts()
{
    QMap<QString, QString> portMap;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        portMap.insert(info.description(),info.portName());
    }
    return portMap;
}

bool SerialCommunication::isOpen()
{
    return serial->isOpen();
}
