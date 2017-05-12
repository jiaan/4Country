#include "chess.h"

Chess::Chess(Board *parent, const Board::State& player, const char id, const QPoint& location)
    :QObject(parent),  m_player(player), m_id(id),m_location(location), pBoard(parent)  {

    pBoard->setBoard(m_location, m_player);
    switch (m_id) {
    case 'B':
        m_name = "兵";
        break;
    case 'C':
        m_name = "车";
        break;
    case 'M':
        m_name = "马";
        break;
    case 'P':
        m_name = "炮";
        break;
    case 'X':
        m_name = "相";
        break;
    case 'S':
        m_name = "士";
        break;
    case 'J':
        m_name = "将";
        break;
    }
}

void Chess::getPrediction()
{
    //pBoard->setPredictionMode(true); // enter prediction mode
    m_prediction += new QPointWrapper(m_location); // click self to cancel

    switch (m_id) {
    case 'B': {
        QList<QPoint> rule;
        if(m_location.y()==15 && //not moved and not blocked
                pBoard->queryBoard(m_location+QPoint(0,-1))==Board::State::EMPTY)
            rule = { QPoint(0,-1), QPoint(0,-2)};
        else if(m_location.y()<14) // passed river
            rule = { QPoint(0,-1), QPoint(1,0), QPoint(-1,0)};
        else
            rule = { QPoint(0,-1)};

        for (auto i:rule)
        {
            QPoint dest(i + m_location);
            if (dest.x() <0 || dest.x() > 18 ||
                    dest.y() < 0 || dest.y() > 18) continue;
            Board::State destState (pBoard->queryBoard(dest));

            if (destState == Board::State::EMPTY ||  // dest is empty or enemy
                    destState == Board::State::PLAYER2 ||
                    destState == Board::State::PLAYER4)
                m_prediction += new QPointWrapper(dest);
        }
    }
        break;

    case 'P': {
        QList<QPoint> directions = {QPoint(0,1) ,QPoint(0,-1),
                                    QPoint(1,0), QPoint(-1,0)};
        for (auto d:directions) {
            auto p = m_location + d;
            while (p.x() >=0 && p.x() < 19 &&
                   p.y() >=0 && p.y() <19) {
                if(pBoard->queryBoard(p)!=Board::State::EMPTY) {
                    p += d;
                    while(p.x() >=0 && p.x() < 19 &&
                          p.y() >=0 && p.y() <19) {
                        if(pBoard->queryBoard(p)==Board::State::EMPTY) {
                            p +=d;
                            continue;
                        } else if(pBoard->queryBoard(p)==Board::State::PLAYER2
                               ||pBoard->queryBoard(p)==Board::State::PLAYER4){
                            m_prediction += new QPointWrapper(p);
                            break;
                        } else  break;
                    }
                    break;
                } else {
                    m_prediction += new QPointWrapper(p);
                    p += d;
                }
            }
        }
    }
        break;

    case 'C' : {
        QList<QPoint> directions = {QPoint(0,1) ,QPoint(0,-1),
                                    QPoint(1,0), QPoint(-1,0)};
        for (auto d:directions) {
            auto p = m_location + d;
            while (p.x() >=0 && p.x() < 19 &&
                   p.y() >=0 && p.y() <19) {
                if (pBoard->queryBoard(p)==Board::State::EMPTY) {
                    m_prediction += new QPointWrapper(p);
                    p += d;
                } else if (pBoard->queryBoard(p)==Board::State::PLAYER2
                           ||pBoard->queryBoard(p)==Board::State::PLAYER4) {
                    m_prediction += new QPointWrapper(p);
                    break;
                } else break;
            }
        }

    }
        break;

    case 'M': {
        QList<QPoint> directions = {QPoint(1,2) ,QPoint(1,-2),
                                    QPoint(-1,2), QPoint(-1,-2),
                                    QPoint(-2,1), QPoint(-2,-1),
                                    QPoint(2,1), QPoint(2,-1)};
        for (auto d:directions) {
            auto p = m_location + d;
            if (p.x() <0 || p.x() > 18 ||
                    p.y() < 0 || p.y() > 18) continue;

            QPoint q = m_location + 0.4*d;

            if (pBoard->queryBoard(q)!=Board::State::EMPTY) continue;
            if (pBoard->queryBoard(p)==Board::State::EMPTY ||
                    pBoard->queryBoard(p)==Board::State::PLAYER2 ||
                    pBoard->queryBoard(p)==Board::State::PLAYER4)
                m_prediction += new QPointWrapper(p);
        }
    }
        break;

    case 'X': {
        QList<QPoint> directions = {QPoint(2,2) ,QPoint(2,-2),
                                    QPoint(-2,2), QPoint(-2,-2)};
        for (auto d:directions) {
            auto p = m_location + d;

            if (p.x() < 4 || p.x() > 13 ||
                    p.y() < 13 || p.y() > 18) continue;

            auto q = m_location + d/2;

            if (pBoard->queryBoard(q)!=Board::State::EMPTY) continue;
            if (pBoard->queryBoard(p)==Board::State::EMPTY ||
                    pBoard->queryBoard(p)==Board::State::PLAYER2 ||
                    pBoard->queryBoard(p)==Board::State::PLAYER4)
                m_prediction += new QPointWrapper(p);
        }
    }
        break;

    case 'S': {
        QList<QPoint> directions = {QPoint(1,1) ,QPoint(1,-1),
                                    QPoint(-1,1), QPoint(-1,-1)};
        for (auto d:directions) {
            auto p = m_location + d;

            if (p.x() < 8 || p.x() > 10 ||
                   p.y() < 16 || p.y() > 18) continue;

            if (pBoard->queryBoard(p)==Board::State::EMPTY ||
                    pBoard->queryBoard(p)==Board::State::PLAYER2 ||
                    pBoard->queryBoard(p)==Board::State::PLAYER4)
                m_prediction += new QPointWrapper(p);
        }
    }
        break;

    case 'J': {
        QList<QPoint> directions = {QPoint(1,0) ,QPoint(-1,0),
                                    QPoint(0,1), QPoint(0,-1)};
        for (auto d:directions) {
            auto p = m_location + d;

            if (p.x() < 8 || p.x() > 10 ||
                   p.y() < 16 || p.y() > 18) continue;

            if (pBoard->queryBoard(p)==Board::State::EMPTY ||
                    pBoard->queryBoard(p)==Board::State::PLAYER2 ||
                    pBoard->queryBoard(p)==Board::State::PLAYER4)
                m_prediction += new QPointWrapper(p);
        }
    }
        break;

    }



    emit predictionChanged();
}


void Chess::moveTo(const QPoint dest)
{
    m_prediction.clear();
    emit predictionChanged();
    //pBoard->setPredictionMode(false); // exit prediciton mode

    if (dest == m_location)  // do nothing
        return;
    pBoard->requestMove_local(m_location.x(),m_location.y(), dest.x(), dest.y());

}


bool Chess::isMine() {
    if(m_player == Board::State::PLAYER1)
        return true;
    else
        return false;
}

QColor Chess::color() {

    int offset;
    switch (m_player) {
    case Board::State::PLAYER1:
        offset =0;
        break;
    case Board::State::PLAYER2:
        offset =1;
        break;
    case Board::State::PLAYER3:
        offset =2;
        break;
    case Board::State::PLAYER4:
        offset =3;
        break;
    default:
        return QColor("white");
    }

    int seat = (pBoard->seat()-1 + offset)%4;

    switch (seat) {
    case 0:
        return QColor("darkorange");
        break;
    case 1:
        return QColor("lightcoral");
        break;
    case 2:
        return QColor("darkturquoise");
        break;
    case 3:
        return QColor("limegreen");
    }
    return QColor("white");
}
