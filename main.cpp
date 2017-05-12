#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "socket.h"
#include "board.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qmlRegisterType<Board>("Board", 1, 0, "Board");

    Socket socket;

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("socket", &socket);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
