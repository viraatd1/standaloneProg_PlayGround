#include "captureddatastore.h"
#include <QByteArray>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QDebug>
#include <QXYSeries>
#include <QTextStream>
#include <QFuture>
#include <QtConcurrent>


const QStringList CapturedDataStore::m_headerKeysList = {"Pre Volt","Pre Current(mA)","Energy setpoint(mJ)","Pre Power(mW)","Pre Impedance","Energy\nDelivered(mJ)",
                                                         "Avg Current(mA)","Avg Volt","Avg Power(mW)","Impedance High","Impedance Low","Treatment Time",
                                                         "Time Limit","Regulation Mode","Samples","Sample time","Interval","Emode","Scount","Tpause"};

#define APPDEF_APPLICATION_RF_IMPEDANCE_DATA_FILE_PATH      "/application/mem/RF/ImpedanceData.json"
#define APPDEF_APPLICATION_CONST_MAX_DATA_POINTS_IN_PULSE   969  /* Since counter is starting from zero */

CapturedDataStore::CapturedDataStore(QObject *parent) : QObject(parent)
  , m_RFImpedanceFilePath(APPDEF_APPLICATION_RF_IMPEDANCE_DATA_FILE_PATH)
  , m_capturedDPCount(0)
  , m_capturedPDGroup("")
  , m_capturedPDDescription("")
  , m_graphLoaded(false)
  , m_seriesHandle(nullptr) /* Initiate to nullptr if at all incase the compiler assigns a garbage and that it might be harmful at derefernce and consumption */
{
    connect(this, &CapturedDataStore::capturedDPSelectedIndexChanged, this, &CapturedDataStore::reloadDataAtSelectedIndex, Qt::QueuedConnection);
    connect(this, &CapturedDataStore::capturedImpedanceGraphPDChanged, this, &CapturedDataStore::processGraphicalData, Qt::QueuedConnection);

    this->initCapturedData();
}

bool CapturedDataStore::graphLoaded() const
{
    return m_graphLoaded;
}

void CapturedDataStore::setGraphLoaded(bool graphLoaded)
{
    if(m_graphLoaded != graphLoaded){
        m_graphLoaded = graphLoaded;
        emit graphLoadedChanged();
    }
}

QString CapturedDataStore::impedanceLegendMetaData() const
{
    return m_ImpedanceLegendMetaData;
}

void CapturedDataStore::setImpedanceLegendMetaData(const QString &ImpedanceLegendMetaData)
{
    if(m_ImpedanceLegendMetaData != ImpedanceLegendMetaData){
        m_ImpedanceLegendMetaData = ImpedanceLegendMetaData;
        emit impedanceLegendMetaDataChanged();
    }
}

QVector<QPointF> CapturedDataStore::capturedDPGraphicalData() const
{
    return m_capturedDPGraphicalData;
}

void CapturedDataStore::setCapturedDPGraphicalData(const QVector<QPointF> &capturedDPGraphicalData)
{
    m_capturedDPGraphicalData.clear();
    m_capturedDPGraphicalData = std::move(capturedDPGraphicalData);
    emit capturedDPGraphicalDataChanged();
}

float CapturedDataStore::yAxisMax() const
{
    return m_yAxisMax;
}

void CapturedDataStore::setYAxisMax(float yAxisMax)
{
    if(m_yAxisMax != yAxisMax){
        m_yAxisMax = yAxisMax;
        emit yAxisMaxChanged();
    }
}

void CapturedDataStore::resetPDUserInputs()
{
    qDebug()<< "Resetting:";
    this->setCapturedPDGroup("");
    this->setCapturedPDDescription("");
    qDebug()<< m_capturedPDDescription << " /" << m_capturedPDGroup;
}

float CapturedDataStore::yAxisMin() const
{
    return m_yAxisMin;
}

void CapturedDataStore::setYAxisMin(float yAxisMin)
{

    if(m_yAxisMin != yAxisMin){
        m_yAxisMin = yAxisMin;
        emit yAxisMinChanged();
    }
}

QVector<float> CapturedDataStore::capturedImpedanceGraphPD() const
{
    return m_capturedImpedanceGraphPD;
}

void CapturedDataStore::setCapturedImpedanceGraphPD(const QVector<float> &capturedImpedanceGraphPD)
{
    m_capturedImpedanceGraphPD.clear();
    m_capturedImpedanceGraphPD = std::move(capturedImpedanceGraphPD);
    emit capturedImpedanceGraphPDChanged();
}

int CapturedDataStore::capturedDPSelectedIndex() const
{
    return m_capturedDPSelectedIndex;
}

void CapturedDataStore::setCapturedDPSelectedIndex(int capturedDPSelectedIndex)
{
    if(m_capturedDPSelectedIndex != capturedDPSelectedIndex){
        m_capturedDPSelectedIndex = capturedDPSelectedIndex;
        emit capturedDPSelectedIndexChanged();
    }
}

QStringList CapturedDataStore::capturedPDGDList() const
{
    return m_capturedPDGDList;
}

int CapturedDataStore::capturedDPCount() const
{
    return m_capturedDPCount;
}

void CapturedDataStore::setCapturedPDGDList(const QStringList &capturedPDGDList)
{
    m_capturedPDGDList.clear();
    m_capturedPDGDList = std::move(capturedPDGDList);
    emit capturedPDGDListChanged();
}

void CapturedDataStore::setCapturedDPCount(int capturedDPCount)
{
    if(m_capturedDPCount != capturedDPCount){
        m_capturedDPCount = capturedDPCount;
        emit capturedDPCountChanged();
    }
}

QVector<float> CapturedDataStore::capturedHeaderValuesList() const
{
    return m_capturedHeaderValuesList;
}

void CapturedDataStore::setCapturedHeaderValuesList(const QVector<float> &headerValuesList)
{
    m_capturedHeaderValuesList.clear();
    m_capturedHeaderValuesList = std::move(headerValuesList);
    emit capturedHeaderValuesListChanged();
}

QStringList CapturedDataStore::headerKeysList()
{
    return m_headerKeysList;
}

void CapturedDataStore::passThroughSeriesPointer(QtCharts::QAbstractSeries *arg_series)
{
    if(!arg_series){
        return;
    }

    QVector<QPointF> lcl_new_datapoints = this->m_capturedDPGraphicalData;
    QtCharts::QXYSeries *xySeries = static_cast<QtCharts::QXYSeries *>(arg_series);
    xySeries->replace(lcl_new_datapoints);
}

void CapturedDataStore::registerSeriesHandle(QtCharts::QAbstractSeries *seriesHandle)
{
    if(!seriesHandle){
        qDebug()<< "Nullptr Handle Received";
    }
    this->m_seriesHandle = seriesHandle;
    this->reloadGraph();
}

void CapturedDataStore::initCapturedData()
{
    /* Internal to the class the data structure is composed of a map
         Key : String : <00000>_<Group>_<Description>_<Time>
         Value : Array of Floats : []
       The representation of these data is different in its context
         Storage Context : JSON Sting key, to value of array
         Progam Variable : QMap<QString, QVector<float>> */
    this->loadCapturedJSONData();
    this->loadCapturedDataModel();
    this->loadCapturedMetaDataModel();
    this->processGraphicalData();
}

void CapturedDataStore::addNewCapturedDPToJSON(QString arg_key, QVector<float> arg_value)
{
    qDebug()<< "Adding New Data Set to Persistent Location With Key:" << arg_key;
    QJsonArray lcl_crtArr;
    for(auto each_val: arg_value){
        lcl_crtArr.append(each_val);
    }
    __rawJSONStoredCapturedData.insert(arg_key, lcl_crtArr);
    this->updateJSONDataFile();
}

/* Asynch operation of writing this data to the file */
void CapturedDataStore::updateJSONDataFile()
{
    QJsonDocument lcl_dp_document;
    QJsonObject lcl_dp_obj_to_write = this->__rawJSONStoredCapturedData;
    lcl_dp_document.setObject(lcl_dp_obj_to_write);

    QByteArray lcl_dp_bytes = lcl_dp_document.toJson(QJsonDocument::Indented);
    QFile lcl_dp_file(this->m_RFImpedanceFilePath.toString());
    if(lcl_dp_file.open( QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)){
        qDebug() <<"[captureddatastore.cpp][updateJSONDataFile]|Updating New Data set to File:" << lcl_dp_file.fileName();
        QTextStream iStream( &lcl_dp_file );
        iStream.setCodec("utf-8");
        iStream << lcl_dp_bytes;
        lcl_dp_file.close();
    } else {
        qDebug() <<"[captureddatastore.cpp][updateJSONDataFile]|Failed to Open File:" << lcl_dp_file.fileName();
    }
}

void CapturedDataStore::loadCapturedJSONData()
{
    if (QFile::exists(m_RFImpedanceFilePath.toString())) {
        qDebug() << "[subscriptionpurchaseHistorymodel.cpp][loadCapturedJSONData]Loading File:" << this->m_RFImpedanceFilePath.toString();

        QFile lcl_settings_file_new(this->m_RFImpedanceFilePath.toString());
        lcl_settings_file_new.open(QIODevice::ReadOnly | QIODevice::Text);
        QString lcl_json_data_in_str_format = lcl_settings_file_new.readAll();
        lcl_settings_file_new.close();

        QJsonParseError lcl_error_handler;
        QJsonDocument lcl_json_doc = QJsonDocument::fromJson(lcl_json_data_in_str_format.toUtf8(), &lcl_error_handler);

        if(lcl_json_doc.isNull()) {
            qDebug() << "[captureddatastore.cpp][loadCapturedJSONData]Impedance File Data is not inconsistent with JSON Protocol, ErrorID:" << lcl_error_handler.errorString();
        } else {
            this->__rawJSONStoredCapturedData = lcl_json_doc.object();
        }
    }
    else {
        qDebug() << "[captureddatastore.cpp][loadCapturedJSONData]File:" << this->m_RFImpedanceFilePath.toString() << " does not exist";
    }
}

void CapturedDataStore::loadCapturedDataModel()
{
    if(__rawJSONStoredCapturedData.empty()){
        qDebug() << "[captureddatastore.cpp][loadCapturedDataModel]Empty Data in the file";
        return;
    }

    foreach (const QString &each_data_point, __rawJSONStoredCapturedData.keys()) {
        auto lcl_loop_dataSet = __rawJSONStoredCapturedData.value(each_data_point).toArray();
        QVector<float> lcl_loop_conv;
        lcl_loop_conv.reserve(lcl_loop_dataSet.size());  // Reserve space for efficiency
        for(int i=0; i< lcl_loop_dataSet.size(); ++i){
            if (lcl_loop_dataSet[i].isDouble()) { // Ensure the value is a double because of JSON to Runtime
                lcl_loop_conv.append(static_cast<float>(lcl_loop_dataSet[i].toDouble()));
            }
        }
        __capturedPDContainer.insert(each_data_point, std::move(lcl_loop_conv));
    }
}

void CapturedDataStore::loadCapturedMetaDataModel()
{
    if(__capturedPDContainer.empty()){
        qDebug() << "[captureddatastore.cpp][loadCapturedMetaDataModel]Empty Data in the Model";
        return;
    }

    qDebug() << "[captureddatastore.cpp][loadCapturedMetaDataModel]Loading Data in the Model";

    /* Meta data UI-1: Total Count */
    this->m_capturedDPCount = __capturedPDContainer.size();

    /* Meta data UI-2: Data Points */
    this->m_capturedPDGDList = __capturedPDContainer.keys();

    /* Meta data UI-3: At the Boot, point it to the last value in the list */
    m_capturedDPSelectedIndex = m_capturedDPCount - 1;

    /* Get the last element from the loaded list */
    auto lcl_last_dataSet = --this->__capturedPDContainer.cend();

    /* Meta data UI-4: Header Values (At the Boot, point it to the last value in the list)*/
    this->m_capturedHeaderValuesList = lcl_last_dataSet.value().mid(0,20);

    /* Meta data UI-5: Impedance Plot data (At the Boot, point it to the last value in the list)*/
    this->m_capturedImpedanceGraphPD = lcl_last_dataSet.value().mid(20,lcl_last_dataSet.value().size() - 1);

    /* Meta data UI-6: Impedance Plot MetaData (At the Boot, point it to the last value in the list)*/
    this->m_ImpedanceLegendMetaData = lcl_last_dataSet.key();
}

void CapturedDataStore::reloadCapturedMetaDataModel()
{
    this->setCapturedDPCount(__capturedPDContainer.size());
    this->setCapturedPDGDList(__capturedPDContainer.keys());
    this->setCapturedDPSelectedIndex(m_capturedDPCount - 1);
}

void CapturedDataStore::reloadDataAtSelectedIndex()
{
    if(m_capturedDPSelectedIndex > this->m_capturedPDGDList.size()){
        return;
    }

    auto lcl_selected_key = m_capturedPDGDList.at(m_capturedDPSelectedIndex);
    QVector<float> lcl_selected_value = __capturedPDContainer[lcl_selected_key];
    this->setCapturedHeaderValuesList(lcl_selected_value.mid(0,20));
    this->setCapturedImpedanceGraphPD(lcl_selected_value.mid(20,lcl_selected_value.size()-1));
    this->setImpedanceLegendMetaData(lcl_selected_key);
}

void CapturedDataStore::processGraphicalData()
{
    if(m_capturedImpedanceGraphPD.empty()){
        qDebug() << "[captureddatastore.cpp][processGraphicalData]Empty Data in the Model";
        return;
    }

    float lcl_minValue = *std::min_element(m_capturedImpedanceGraphPD.constBegin(), m_capturedImpedanceGraphPD.constEnd());
    float lcl_maxValue = *std::max_element(m_capturedImpedanceGraphPD.constBegin(), m_capturedImpedanceGraphPD.constEnd());

    QVector<QPointF> lcl_processedData;

    // Assuming time is sequential from 0 to 949 for each data point
    for (int lp_lcl_i = 0; lp_lcl_i < m_capturedImpedanceGraphPD.size(); ++lp_lcl_i) {
        lcl_processedData.append(QPointF(lp_lcl_i, m_capturedImpedanceGraphPD[lp_lcl_i]));
    }

    this->setYAxisMin(lcl_minValue);
    this->setYAxisMax(lcl_maxValue);
    this->setCapturedDPGraphicalData(lcl_processedData);
    this->reloadGraph();
}

void CapturedDataStore::reloadGraph()
{
    if(!m_seriesHandle){
        return;
    }

    QtCharts::QXYSeries *lcl_xySeriesPtr = static_cast<QtCharts::QXYSeries *>(this->m_seriesHandle);

    // Example of asynchronous update using QtConcurrent
    QFuture<void> lcl_futureObj = QtConcurrent::run([lcl_xySeriesPtr, &refVar = m_capturedDPGraphicalData]() {
        lcl_xySeriesPtr->replace(refVar);
    });

    //xySeries->replace(this->m_capturedDPGraphicalData);
    this->setGraphLoaded(true);
}

CapturedDataStore *CapturedDataStore::instance()
{
    static CapturedDataStore *m_instance;
    if(!m_instance){
        m_instance = new CapturedDataStore();
    }

    return m_instance;
}

QString CapturedDataStore::capturedPDTime() const
{
    return m_capturedPDTime;
}

void CapturedDataStore::setCapturedPDTime(const QString &capturedPDTime)
{
    m_capturedPDTime = capturedPDTime;
}

QString CapturedDataStore::capturedPDGroup() const
{
    return m_capturedPDGroup;
}

void CapturedDataStore::setCapturedPDGroup(const QString &capturedPDGroup)
{
    if(m_capturedPDGroup.compare(capturedPDGroup) != 0){
        m_capturedPDGroup = capturedPDGroup;
        emit capturedPDGroupChanged();
    }
}

QString CapturedDataStore::capturedPDDescription() const
{
    return m_capturedPDDescription;
}

void CapturedDataStore::setCapturedPDDescription(const QString &capturedPDDescription)
{
    if(m_capturedPDDescription.compare(capturedPDDescription) != 0){
        m_capturedPDDescription = capturedPDDescription;
        emit capturedPDDescriptionChanged();
    }
}

int CapturedDataStore::addDataSet(QVector<float> &arg_dataSet)
{
    qDebug()<< "Adding New Data Set:" << arg_dataSet.size();

    /* First Take the ownership of the data, because the serial is running in seperate thread and that it might over-ride before this class even process that data*/
    const QVector<float> lcl_rawDataCopy = std::move(arg_dataSet);
    const auto lcl_rawDataSetSize = lcl_rawDataCopy.count();

    /* Extract the Meta Information needed by the UI and packet to the store */
    if(lcl_rawDataSetSize != (APPDEF_APPLICATION_CONST_MAX_DATA_POINTS_IN_PULSE+1)){
        qDebug()<< "Improper Data Set Size:" << lcl_rawDataSetSize;
        return 101;
    }

    /* Validation : Check for data integrity */
    //if(){
    //    return 102;
    //}

    /* At this exec point, everything looks good, so first add it to the internal data structure, before updating on the UI */
    QString lcl_formtKey = QStringLiteral("%1_%2_%3").arg((m_capturedDPCount+1), 6, 10, QLatin1Char('0')).arg(m_capturedPDGroup).arg(m_capturedPDDescription);

    /* Generate & add the json data to the file in async mode, since there is no need to wait in here */
    this->addNewCapturedDPToJSON(lcl_formtKey, lcl_rawDataCopy);

    /* Add this to the local class level model */
    __capturedPDContainer.insert(lcl_formtKey, std::move(lcl_rawDataCopy));

    /* ToDo : There could be a better way of just updating the new entry using begin list model */
    this->reloadCapturedMetaDataModel();

    /* Reset the naming for the next data set */
    this->resetPDUserInputs();
}
