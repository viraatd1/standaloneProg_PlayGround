#ifndef SERIALREADER_H
#define SERIALREADER_H

#include "captureddatastore.h"

#include <QObject>
#include <QSerialPort>
#include <QThread>
#include <QVector>
#include <QPointF>
#include <QString>
#include <QDateTime>
#include <QtGlobal>
#include <QMap>
#include <QVariantList>
#include <QVariant>
#include <QMutex>



using namespace std;

#define APPDEF_APPLICATION_CONST_MAX_DATA_POINTS_IN_PULSE 969  /* Since counter is starting from zero */


class SerialReader : public QObject
{
    Q_OBJECT

public:
    explicit SerialReader(QObject *parent = nullptr);
    ~SerialReader();

    Q_INVOKABLE void startReading();
    Q_INVOKABLE void stopReading();
    Q_INVOKABLE void writeData(const QByteArray &data);
    bool readingEnabled() const;
    void registerDataConsumer(CapturedDataStore *arg_consumer);

signals:
    void dataReceived(const QMap<int, QVector<QPointF>> &dataMap);
    void errorOccurred(const QString &error);

private slots:
    void readData();
    void setReadingEnabled(bool readingEnabled);

private:
    /* Variables : Serial Port Related*/
    QSerialPort *m_serialPort;
    QByteArray m_receivedDataBuffer;

    QThread m_thread;
    QMutex m_receivedDataBufferMutex;

    CapturedDataStore *m_dataConsumer;

    /* Variables : UI Facing Set/Get & Application Required */
    bool m_readingEnabled;                              /* Protocol to read data */

    /* Variables : Protocol Definition Variables */
    quint16 m_dataCounter;                              /* Protocol to read data */
    QVector<float> m_rawDataSet;                        /* Captured Individual Data Point */

    void setCapturedDataPoints(const QMap<int, QVector<float> > &capturedDataPoints);


    void cleanBuffer(int arg_position, int arg_length);

    /* Data Received and Storage Depends on the contract between the RF-167-UI Protocol
        Received data is parsed according to the protocol and that it is defined */
    void parse_RFModuleContract();

    /* Debug Functions, removed at the time production software package */
    void parse_SimulationByTimeBound();
};

#endif // SERIALREADER_H
