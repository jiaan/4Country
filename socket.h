#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <QtNetwork>
#include <QString>
#include <QList>

class Player : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(uint seat READ seat NOTIFY seatChanged)
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(int score READ score NOTIFY scoreChanged)

public:
    explicit Player(QObject *parent = 0): QObject(parent) {}
    QString name() const { return m_name;}
    uint seat() const { return m_seat; }
    bool ready() const {return m_ready;}
    int score() const { return m_score; }
    friend class Socket;

private:
    QHostAddress m_ip;
    quint16 m_port;
    QString m_name = "unknown";
    uint m_seat = 0; // 0 means unseated
    bool m_ready = false;
    int m_score = 0;

signals:
    void seatChanged();
    void readyChanged();
    void scoreChanged();

public slots:
};

class Socket : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected MEMBER m_connected NOTIFY connectedChanged)
    Q_PROPERTY(bool start MEMBER m_start NOTIFY startChanged)
    Q_PROPERTY(bool turn MEMBER m_turn NOTIFY turnChanged)
    Q_PROPERTY(QList<QObject*> players MEMBER m_players NOTIFY playersChanged)
    Q_PROPERTY(QStringList seat READ seat NOTIFY seatChanged)
    Q_PROPERTY(QObject* socket READ socket CONSTANT)
    Q_PROPERTY(QString name MEMBER m_name CONSTANT)
    Q_PROPERTY(QString chatlog MEMBER m_chatlog NOTIFY chatlogChanged)

public:
    explicit Socket(QObject *parent = 0);
    QStringList seat() const;
    uint mySeat() { return m_seat;}


private:
    QString m_name;
    uint m_id = 0; // id is the unique identifier
    bool m_server;
    bool m_connected = false;
    bool m_start = false;
    bool m_turn = false; // indicate if its my turn
    uint m_step = 0; // keep track of steps
    uint m_seat; // my seat
    QList<uint> m_player_left = {1,2,3,4};
    QObject* socket() { return this;}
    QString m_chatlog;


    QUdpSocket* m_sock;
    QList<QObject*> m_players; //  server is always the first player

    void sendData(const QByteArray &data);
    void addPoint(uint seat, int point);
    void resetGame();

private slots:
    void processDatagrams();

    // void setUpTurns(const int start);
    // void disconnectSocket();

public slots:
    void setName(const QString& name) { m_name = name; } // MEMBER is better
    void setClient(const QString& ip, quint16 port);
    bool setServer(quint16 port);
    void setSeat(const uint s);
    void setReady(bool r);
    void updateLocation(int sx, int sy, int dx, int dy);
    void removePlayer(uint s);
    void chat(const QString& content);




signals:
    void playersChanged();
    void connectedChanged();
    void seatChanged();
    void startChanged();
    void turnChanged();
    void chatlogChanged();
    void changeLocation(int sx, int sy, int dx, int dy);
};




#endif // SOCKET_H
