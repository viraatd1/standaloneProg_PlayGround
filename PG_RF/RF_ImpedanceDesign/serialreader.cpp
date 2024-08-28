#include <QDebug>
#include <QDateTime>
#include "serialreader.h"
#include <QSerialPortInfo>


SerialReader::SerialReader(QObject *parent)
    : QObject(parent),
      m_serialPort(new QSerialPort(nullptr)), /* construct without parent so as to move to different thread */
      m_readingEnabled(true),
      m_dataCounter(0),
      m_dataConsumer(nullptr)
{
    /* set the serial port parameters  ttymxc1 ttyUSB0*/
    m_serialPort->setPortName("/dev/ttymxc1");      /* The terminal character device file that is inherited from the parent and is assigned for RS232 serial communication*/

    if(!m_serialPort->setBaudRate(349000, QSerialPort::Input)){
        qDebug() << "Serial port error: Could not set BAUDRATE" << m_serialPort->portName() << " : " << m_serialPort->errorString();
    }

    m_serialPort->setBaudRate(QSerialPort::BaudRate::Baud115200, QSerialPort::AllDirections);
    m_serialPort->setDataBits(QSerialPort::DataBits::Data8);
    m_serialPort->setParity(QSerialPort::Parity::NoParity);
    m_serialPort->setStopBits(QSerialPort::StopBits::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    /* open the port for read and write */
    if (m_serialPort->open(QIODevice::ReadWrite)) {
        connect(&m_thread, &QThread::finished, m_serialPort, &QObject::deleteLater);
        connect(m_serialPort, &QSerialPort::readyRead, this, &SerialReader::readData, Qt::QueuedConnection);
    } else {
        qDebug() << "Serial port error: Could not open" << m_serialPort->portName() << " : " << m_serialPort->errorString();
    }

    m_rawDataSet.clear();

    m_serialPort->moveToThread(&m_thread);
    //m_serialPort->setParent(this); /* Get this thread a parent so as to manage efficiently */

    m_thread.start();

}

SerialReader::~SerialReader()
{
    stopReading();
    m_thread.quit();
    m_thread.wait();
}

void SerialReader::startReading()
{
    if (m_readingEnabled) {
        qDebug() << "Already reading from serial port.";
        return;
    }

    if (!m_serialPort->open(QIODevice::ReadOnly)) {
        qDebug() << m_serialPort->errorString();
        emit errorOccurred("Failed to open port " + m_serialPort->portName() + ": " + m_serialPort->errorString());
        return;
    } else {
        qDebug() << "Serial port Opened" << m_serialPort->portName();
    }

    m_readingEnabled = true;
}

void SerialReader::stopReading()
{
    if (m_readingEnabled) {
        m_serialPort->close();
        m_receivedDataBuffer.clear();
        m_readingEnabled = false;
    }
}

void SerialReader::writeData(const QByteArray &data)
{
    if (m_serialPort->isOpen() && m_serialPort->isWritable()) {
        m_serialPort->write(data);
        m_serialPort->waitForBytesWritten();
    } else {
        emit errorOccurred("Serial port not open or not writable.");
    }
}

void SerialReader::readData()
{
    if (!m_readingEnabled) {
        qDebug() << "Reading Not Enabled, Discarding Incoming Data:" << m_serialPort->readAll();
        return;
    }

    /* Always append the data so as to not messup the order */
    m_receivedDataBuffer.append(m_serialPort->readAll());

    //this->parse_RFModuleContract();
    this->parse_SimulationByTimeBound();
}

bool SerialReader::readingEnabled() const
{
    return m_readingEnabled;
}

void SerialReader::registerDataConsumer(CapturedDataStore *arg_consumer)
{
  this->m_dataConsumer = arg_consumer;
}

void SerialReader::setReadingEnabled(bool readingEnabled)
{
    m_readingEnabled = readingEnabled;
}

void SerialReader::cleanBuffer(int arg_position, int arg_length)
{
    QMutexLocker lcl_bufferLocker(&m_receivedDataBufferMutex);

    /* position checking is done by qt-API internally, no need to check explicitly */
    m_receivedDataBuffer.remove(arg_position, arg_length);
}

void SerialReader::parse_RFModuleContract()
{


//    /* ---test code --- */
//    foreach (auto x, m_receivedDataBuffer) {
//        qDebug() << "each " << x <<" ===> " << static_cast<unsigned char>(x);
//    }
//    m_receivedDataBuffer.clear();
//    return;


    /* Contract-1: RawByte Detection */
    static constexpr int data_threshold = (970 *2);

    if(m_receivedDataBuffer.size() >= data_threshold){

        QByteArray lcl_reqDataSet = m_receivedDataBuffer.left(data_threshold);
        QVector<uint16_t> dataPoints;

        /* Pass this data to the consumer, that provides asynchronous call functionality */
        qDebug()<< lcl_reqDataSet;

        if (lcl_reqDataSet.size() % 2 != 0) {
            qDebug() << "Error: Received data size is not even.";
            return;
        }

        // Clear the vector to ensure it's empty before storing new data
        dataPoints.clear();

        // Interpret data in pairs of bytes as uint16_t
        for (int i = 0; i < lcl_reqDataSet.size(); i += 2) {
            uint16_t value = static_cast<uint16_t>((static_cast<uint8_t>(lcl_reqDataSet[i]) << 8) |
                                                   (static_cast<uint8_t>(lcl_reqDataSet[i + 1])));
            dataPoints.append(value);
        }

        qDebug()<<"viraat===>" <<dataPoints;



        /* We assume that the previous call, sucessfully used the data, so we clear the buffer here, so as to identify the next data set correctly */
        this->cleanBuffer(0, data_threshold);
        return;
    }
    else { /* Either Log or remove for performance */
        qDebug() << "received" << m_receivedDataBuffer.size();
        qDebug() << m_receivedDataBuffer;
        qDebug() << "-----------------";
    }

    return;
}

void SerialReader::parse_SimulationByTimeBound()
{
    QByteArray lcl_receivedData = m_receivedDataBuffer;
    QString lcl_formattedString(lcl_receivedData);
    bool lcl_conversionSucessful(true);     /* setting(assuming) true always */
    float lcl_convertedDataPointValue = lcl_formattedString.toFloat(&lcl_conversionSucessful);

    if(!lcl_conversionSucessful){
        qDebug() << "Received Data is not in accordance with the Protocol:" << lcl_receivedData ;
        return;
    }

    qDebug() << "Debug:Counter:" << m_dataCounter << "|Rx: " << lcl_receivedData << "|Converted:" << lcl_convertedDataPointValue;

    /* Assuming Data will be received in numerical order of 970 Points */
    if((m_dataCounter > 0) && (m_dataCounter < APPDEF_APPLICATION_CONST_MAX_DATA_POINTS_IN_PULSE)){
        //qDebug() << "Adding Data Point: " << m_dataCounter << ",Value:"<< lcl_convertedDataPointValue;
        m_rawDataSet.insert(m_dataCounter, lcl_convertedDataPointValue);
        m_dataCounter++;
    }
    else if (m_dataCounter == 0) {
        qDebug() << "DP-START:" << m_dataCounter << ",Value:"<< lcl_convertedDataPointValue;
        m_rawDataSet.insert(m_dataCounter, lcl_convertedDataPointValue);
        m_dataCounter++;
    }
    else if (m_dataCounter == APPDEF_APPLICATION_CONST_MAX_DATA_POINTS_IN_PULSE) {
        qDebug() << "DP-END:" << m_dataCounter << ",Value:"<< lcl_convertedDataPointValue;
        m_rawDataSet.insert(m_dataCounter, lcl_convertedDataPointValue);

        /* Send the Data to the Consumer */
        this->m_dataConsumer->addDataSet(m_rawDataSet);

        /* Clean for the next set */
        m_dataCounter = 0;
        m_rawDataSet.clear();
    }
    else {
        qDebug() << "Unexpected Counter value: " << m_dataCounter << ", for Data:"<< m_serialPort->readAll();
    }

    /* Stored locally, clean it */
    m_receivedDataBuffer.clear();


    // Example: Use a timestamp as key (adjust as per your data format)
    // int timestamp = QDateTime::currentMSecsSinceEpoch(); // Replace with actual timestamp
}
