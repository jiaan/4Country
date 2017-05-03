#include <QGuiApplication>
#include <QQuickView>

#include "board.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    qmlRegisterType<Board>("Board", 1, 0, "Board");

    QQuickView *view = new QQuickView;
    view->setSource(QUrl::fromLocalFile(":/main.qml"));
    view->show();

    return app.exec();
}
