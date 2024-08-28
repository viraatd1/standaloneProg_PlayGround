#include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QFontDatabase>

#define APPDEF_APPLICATION_FONT ":/fonts/HelveticaMedCd.ttf"
#define APPDEF_APPLICATION_DEFAULT_FONT "HelveticaNeue MediumCond"

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    // Load fonts - "HelveticaNeue MediumCond"
    if (QFontDatabase::addApplicationFont(APPDEF_APPLICATION_FONT) == -1)
        qDebug() << "[main.cpp][main]| Failed to load Application font";
    else
        qDebug() << "[main.cpp][main]| Application font successfully loaded";

    /* Set the default application font
      With this, there is now no need to explicitly specify the font.family attribute
       for individual components unless it has tobe a different font style other than the default */
    app.setFont(QFont(APPDEF_APPLICATION_DEFAULT_FONT));

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
