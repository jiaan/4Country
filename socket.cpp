#include "socket.h"
#include <QTextStream>
#include <QByteArray>

Socket::Socket(QObject *parent) :QObject(parent)
{
    m_sock = new QUdpSocket;
    connect(m_sock, SIGNAL(readyRead()), this, SLOT(processDatagrams()));

}


void Socket::setClient(const QString &ip, quint16 port) {
    m_server = false;

    auto server = new Player; // we don't know the name yet
    server->m_ip  = QHostAddress(ip);
    server->m_port = port;
    m_players += server;

    m_sock->bind();
    sendData('_'+m_name.toUtf8());
}


bool Socket::setServer(quint16 port) {
    m_sock->abort();

    m_server = true;
    m_connected = true;

    auto server = new Player;
    server->m_ip  = QHostAddress::LocalHost;
    server->m_port = port;
    server->m_name = m_name;

    m_players += server;
    emit playersChanged();

    return (m_sock->bind(port));
}


void Socket::processDatagrams() {
    QTextStream out(stdout);
    out << "Get connection\n";

    QByteArray datagram;
    QHostAddress ip;
    quint16 port;
    datagram.resize(m_sock->pendingDatagramSize());
    m_sock->readDatagram(datagram.data(), datagram.size(), &ip, &port);
    out << "data is: " << datagram << endl;

    char identifier = datagram[0];

    switch (identifier)
    {
    case '_': // initiation
        if (m_server) {
            // forward all players names and seat to the sender
            for (auto i=0; i < m_players.size(); i++) {
                auto p = qobject_cast<Player*>(m_players[i]);
                m_sock->writeDatagram('_'+p->m_name.toUtf8(), ip, port);
                m_sock->writeDatagram('s'+QByteArray::number(i)+
                                      QByteArray::number(p->m_seat), ip, port);
                if(p->m_ready)
                    m_sock->writeDatagram('r'+QByteArray::number(i), ip, port);
            }
            // register sender
            auto client = new Player;
            client->m_ip  = ip;
            client->m_port = port;
            client->m_name = datagram.remove(0,1);
            m_players += client;
            emit playersChanged();

            // tell others about the comer (include the sender)
            sendData('+'+datagram);
        } else {
            // set up host's name and our id
            if (!m_connected) {
                m_connected = true;
                emit connectedChanged();

                // first data is host's name
                auto p = qobject_cast<Player*>(m_players[0]);
                p->m_name = datagram.remove(0,1);
                emit playersChanged();
                m_id++;
            } else {
                auto peer = new Player;
                peer->m_name = datagram.remove(0,1);
                m_players += peer;
                emit playersChanged();
                m_id++; // when initializing finishes, m_id is our id
            }
        }
        break;

    case '+': // add player
    {
        auto peer = new Player;
        peer->m_name = datagram.remove(0,1);
        m_players += peer;
        emit playersChanged();      
    }
        break;

    case 's': // set seat: "s12" means player 1 seats at 2
    {
        if (m_server) sendData(datagram);
        uint id = QChar(datagram[1]).digitValue();
        uint s = QChar(datagram[2]).digitValue();
        auto p = qobject_cast<Player*>(m_players[id]);

        if(id==m_id) m_seat = s;
        p->m_seat = s;
        emit seatChanged();
        p->seatChanged();

    }
        break;

    case 'r': // ready state changed
    {
        uint id = QChar(datagram[1]).digitValue();
        auto p = qobject_cast<Player*>(m_players[id]);
        p->m_ready = !p->m_ready;
        p->readyChanged();

        if (m_server) {
            sendData(datagram);

            int count;
            uint start;
            for(count=0; count< m_players.size(); count++) {
                auto p = qobject_cast<Player*>(m_players[count]);
                if(p->m_ready) {
                    if(p->m_seat == 1) start = count;
                } else
                    break;
            }
            if(count==4) {
                m_start = true;
                emit startChanged();


                if(start == m_id) {
                    m_turn = true;
                    emit turnChanged();
                }
                sendData('S'+QByteArray::number(start));
            }
        }
    }
        break;

    case 'S': // game start, S1 means start from player 1
    {
        m_start = true;
        emit startChanged();

        if((uint)QChar(datagram[1]).digitValue() == m_id) {
            m_turn = true;
            emit turnChanged();
        }
    }
        break;

    case 'M': // Msxsydxdy move chess at (sx,sy) to (dx,dy)
    {
        if(m_server) sendData(datagram);

        auto sx = datagram.mid(1,2).toInt();
        auto sy = datagram.mid(3,2).toInt();
        auto dx = datagram.mid(5,2).toInt();
        auto dy = datagram.mid(7,2).toInt();
        emit changeLocation(sx, sy, dx, dy);

        m_step ++;
        uint next_player = m_step%4 +1;
        while(!m_player_left.contains(next_player)) {
            m_step ++;
            next_player = m_step%4 +1;
        }

            if(next_player == m_seat) {
                m_turn = true;
                emit turnChanged();
            } else {
                m_turn = false;
                emit turnChanged();
            }




    }
        break;

    case 'c': // chat
    {
        if(m_server) sendData(datagram);
        QTextStream out(&m_chatlog);
        uint id = QChar(datagram[1]).digitValue();
        auto p = qobject_cast<Player*>(m_players[id]);

        out << p->m_name << ": " <<datagram.remove(0,2) << endl;
        emit chatlogChanged();
    }
        break;
//    case 'K':
//        disconnectSocket();
//        break;



    }


}

//void Socket::disconnectSocket() {
//    m_sock->close();
//    dest.clear();
//    m_players.clear();
//    playersChanged();
//    m_server = false;
//    serverChanged();
//}

void Socket::sendData(const QByteArray &data) {
    if(m_server) { //server should sent to all clients
        auto i = ++m_players.begin();
        while(i != m_players.end()) {
            auto p = qobject_cast<Player*>(*i);
            m_sock->writeDatagram(data,p->m_ip,p->m_port);
            ++i;
        }
    } else { // client only need to send to server
        auto p = qobject_cast<Player*>(m_players[0]);
        m_sock->writeDatagram(data, p->m_ip, p->m_port);
    }
}

void Socket::setSeat(const uint s) {
    if (s > 4) return; // ???
    if (m_server) {
        auto p = qobject_cast<Player*>(m_players[0]);
        m_seat = s;
        p->m_seat = s;
        p->seatChanged();
        emit seatChanged();
    }
    sendData('s'+QByteArray::number(m_id)+QByteArray::number(s));
     // TODO: problem for m_id >= 10
}

void Socket::setReady(bool r) {
    auto p = qobject_cast<Player*>(m_players[m_id]);
    if(r!=p->m_ready) {
        if(m_server) {
            p->m_ready = !p->m_ready;
            p->readyChanged();
        }
        sendData('r'+QByteArray::number(m_id));

        if (m_server) {

            int count;
            uint start;
            for(count=0; count< m_players.size(); count++) {
                auto p = qobject_cast<Player*>(m_players[count]);
                if(p->m_ready) {
                    if(p->m_seat == 1) start = count;
                } else
                    break;
            }
            if(count==4) {
                m_start = true;
                emit startChanged();

                if(start == m_id) {
                    m_turn = true;
                    emit turnChanged();
                }
                sendData('S'+QByteArray::number(start));
            }
        }
    }

}

QStringList Socket::seat() const {
    QStringList s;
    for (uint i = 1; i<5; i++ ) {
        QString name;
        for (auto j:m_players) {
            auto p = qobject_cast<Player*>(j);
            if (p->seat() == i)
                name = p->name();
        }
        s << name;
    }
    return s;
}

void Socket::updateLocation(int sx, int sy, int dx, int dy) {
    if(m_server) {
        emit changeLocation(sx, sy, dx, dy);

        m_step ++;
        uint next_player = m_step%4 +1;
        while(!m_player_left.contains(next_player)) {
            m_step ++;
            next_player = m_step%4 +1;
        }

        if(next_player == m_seat) {
            m_turn = true;
            emit turnChanged();
        } else {
            m_turn = false;
            emit turnChanged();
        }

    }
    sendData('M'+
             QByteArray::number(sx).rightJustified(2,'0')+
             QByteArray::number(sy).rightJustified(2,'0')+
             QByteArray::number(dx).rightJustified(2,'0')+
             QByteArray::number(dy).rightJustified(2,'0'));

}

void Socket::removePlayer(uint s) {
    m_player_left.removeAt(s-1);

    if(!m_player_left.contains(2) && !m_player_left.contains(4)) {
        addPoint(1,2);
        addPoint(3,2);
        resetGame();
    } else if (!m_player_left.contains(1) && !m_player_left.contains(3)) {
        addPoint(2,2);
        addPoint(4,2);
        resetGame();
    }

}

void Socket::addPoint(uint seat, int point) {
    for(auto i:m_players) {
        auto p = qobject_cast<Player*>(i);
        if(p->m_seat == seat) {

            p->m_score += point;
            break;
        }
    }
}

void Socket::resetGame() {
    m_player_left ={1,2,3,4};
    m_step =0;
    m_turn = false;
    m_start = false;

    m_seat =0;
    emit startChanged();
    for(auto i:m_players) {
            auto p = qobject_cast<Player*>(i);
            p->m_seat = 0;
            p->m_ready = false;
            p->seatChanged();
            p->readyChanged();
            p->scoreChanged();
        }
    emit seatChanged();
    emit turnChanged();
}

void Socket::chat(const QString &content) {
    if(m_server) {
        QTextStream out(&m_chatlog);
        out << m_name << ": " << content << endl;
        emit chatlogChanged();
    }
    sendData('c'+ QByteArray::number(m_id) +content.toUtf8());
}
