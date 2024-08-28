#ifndef CAPTUREDDATASTORE_H
#define CAPTUREDDATASTORE_H
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
#include <QJsonObject>
#include <QJsonValue>
#include <QAbstractSeries>

/**
  This is the class that holds responsibility to load, update and add data from various sources
  Also, this will at any point provides API's to fetch data to different sources.
 */
class CapturedDataStore : public QObject
{
    Q_OBJECT

    /* Header API's */
    Q_PROPERTY(int capturedDPCount READ capturedDPCount NOTIFY capturedDPCountChanged)
    Q_PROPERTY(int capturedDPSelectedIndex READ capturedDPSelectedIndex WRITE setCapturedDPSelectedIndex NOTIFY capturedDPSelectedIndexChanged)
    Q_PROPERTY(bool graphLoaded READ graphLoaded WRITE setGraphLoaded NOTIFY graphLoadedChanged)
    Q_PROPERTY(QString capturedPDGroup READ capturedPDGroup WRITE setCapturedPDGroup NOTIFY capturedPDGroupChanged)
    Q_PROPERTY(QString capturedPDDescription READ capturedPDDescription WRITE setCapturedPDDescription NOTIFY capturedPDDescriptionChanged)
    Q_PROPERTY(QString impedanceLegendMetaData READ impedanceLegendMetaData WRITE setImpedanceLegendMetaData NOTIFY impedanceLegendMetaDataChanged)
    Q_PROPERTY(QVector<float> capturedHeaderValuesList READ capturedHeaderValuesList NOTIFY capturedHeaderValuesListChanged)
    Q_PROPERTY(QStringList capturedPDGDList READ capturedPDGDList NOTIFY capturedPDGDListChanged)

    /* Graph API's */
    Q_PROPERTY(float yAxisMin READ yAxisMin WRITE setYAxisMin NOTIFY yAxisMinChanged)
    Q_PROPERTY(float yAxisMax READ yAxisMax WRITE setYAxisMax NOTIFY yAxisMaxChanged)
    Q_PROPERTY(QVector<QPointF> capturedDPGraphicalData READ capturedDPGraphicalData WRITE setCapturedDPGraphicalData NOTIFY capturedDPGraphicalDataChanged)

public:

    /* Singleton Instance */
    static CapturedDataStore* instance();

    /* Public Getter API's */
    QString capturedPDTime() const;
    QString capturedPDGroup() const;
    QString capturedPDDescription() const;
    QVector<float> capturedPD() const;
    QVector<float> capturedHeaderValuesList() const;
    QStringList capturedPDGDList() const;
    int capturedDPCount() const;
    int capturedDPSelectedIndex() const;
    QVector<float> capturedImpedanceGraphPD() const;
    float yAxisMin() const;
    float yAxisMax() const;
    QVector<QPointF> capturedDPGraphicalData() const;
    QString impedanceLegendMetaData() const;
    bool graphLoaded() const;

    /* QML Expose API's */
    Q_INVOKABLE static QStringList headerKeysList();
    Q_INVOKABLE void passThroughSeriesPointer(QtCharts::QAbstractSeries *arg_series);
    Q_INVOKABLE void registerSeriesHandle(QtCharts::QAbstractSeries *seriesHandle);

    /* Public Modifying API's */
    int addDataSet(QVector<float> &arg_dataSet);

signals:
    void capturedHeaderValuesListChanged();
    void capturedPDGroupChanged();
    void capturedPDDescriptionChanged();
    void capturedPDDescriptionListChanged();
    void capturedDPCountChanged();
    void capturedPDGDListChanged();
    void capturedDPSelectedIndexChanged();
    void capturedImpedanceGraphPDChanged();
    void yAxisMinChanged();
    void yAxisMaxChanged();
    void capturedDPGraphicalDataChanged();
    void impedanceLegendMetaDataChanged();
    void graphLoadedChanged();

private:
    CapturedDataStore(QObject *parent = nullptr);

    /* Variables : Exposed UI Related */
    static const QStringList m_headerKeysList;          /* String List of Header keys */
    int m_capturedDPCount;                              /* Count of Total Captured Data Points */
    int m_capturedDPSelectedIndex;                      /* User Selected Index from the UI, on boot it points to the last index */
    QString m_capturedPDTime;                           /* Captured Point Time */
    QString m_capturedPDGroup;                          /* Captured Point GroupName */
    QString m_capturedPDDescription;                    /* Captured Point Point Description */
    QString m_ImpedanceLegendMetaData;                  /* Captured Point Point Description */
    QVector<float> m_capturedHeaderValuesList;          /* String List of captured Pulse Header Values */
    QStringList m_capturedPDGDList;                     /* String List of captured Pulse Group with Description List */
    QVector<float> m_capturedImpedanceGraphPD;          /* Captured Data Points Related to the Impedance Graph */
    bool m_graphLoaded;                                 /* Flag Variable the sets the status of loading the plot */

    /* Variables : Exposed and Graph Related */
    QtCharts::QAbstractSeries* m_seriesHandle;          /* Handle from the QML */
    QVector<QPointF> m_capturedDPGraphicalData;         /* Model representing graph data */
    float m_yAxisMin;                                   /* Calculated Min for the Y-axis */
    float m_yAxisMax;                                   /* Calculated Max for the Y-axis */

    /* Variables : Internal Data Structure of the Class */
    QMap<QString, QVector<float>> __capturedPDContainer;
    QJsonObject __rawJSONStoredCapturedData;

    /* Variables : Files */
    QUrl m_RFImpedanceFilePath;

    /* Called upon initialization */
    void initCapturedData();

    /* Async Operation, DO NOT PASS ANY REFERENCES */
    void addNewCapturedDPToJSON(QString arg_key, QVector<float> arg_value/*QJsonValue &arg_value*/);
    void updateJSONDataFile();

    /* Load JSON Data */
    void loadCapturedJSONData();

    /* Load Model Data into the Class modules */
    void loadCapturedDataModel();
    void loadCapturedMetaDataModel();
    void reloadCapturedMetaDataModel();
    void reloadDataAtSelectedIndex();
    void processGraphicalData();
    void reloadGraph();

    void setCapturedPDTime(const QString &capturedPDTime);
    void setCapturedPDGroup(const QString &capturedPDGroup);
    void setCapturedPDDescription(const QString &capturedPDDescription);
    void setCapturedHeaderValuesList(const QVector<float> &headerValuesList);
    void setCapturedPDGDList(const QStringList &capturedPDGDList);
    void setCapturedDPCount(int capturedDPCount);
    void setCapturedDPSelectedIndex(int capturedDPSelectedIndex);
    void setCapturedImpedanceGraphPD(const QVector<float> &capturedImpedanceGraphPD);
    void setCapturedDPGraphicalData(const QVector<QPointF> &capturedDPGraphicalData);
    void setYAxisMin(float yAxisMin);
    void setYAxisMax(float yAxisMax);
    void setImpedanceLegendMetaData(const QString &impedanceLegendMetaData);
    void setGraphLoaded(bool graphLoaded);

    void resetPDUserInputs();
};

#endif // CAPTUREDDATASTORE_H
