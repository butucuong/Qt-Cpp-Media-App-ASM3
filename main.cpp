#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QMultimedia>

#include "playlistmodel.h"
#include "player.h"
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    qRegisterMetaType<QMediaPlaylist*>("QMediaPlaylist*");

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    Player player;
    engine.rootContext()->setContextProperty("playlistModel",player.m_playlistModel); //set context for playlistModel
    engine.rootContext()->setContextProperty("player",player.m_player);               //set context for QMediaPlayer object
    engine.rootContext()->setContextProperty("m_player",&player);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
