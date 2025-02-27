/*
 * QCoreApplication - for non-graphical application.
 * QGuiApplication  - for graphical applications that DO NOT use Qt Widgets.
 * QApplication     - for graphical applications that USE Qt Widgets.
 */
#include <QGuiApplication>

#include <QQmlApplicationEngine>
#include <touchstonedata.h>

int main(int argc, char *argv[]) {
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<TouchstoneData>("TestApp.Data", 1, 0, "TouchstoneData");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/TestApp.qml")));

    return app.exec();
}
