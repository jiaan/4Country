#include "board.h"
#include "chess.h"
#include <QPainter>


Board::Board(QQuickItem *parent) :QQuickPaintedItem(parent)
{
    ini();
    m_chess += new Chess(this, State::PLAYER1, "1兵", QPoint(2,3));
    m_chess += new Chess(this, State::PLAYER2, "2兵", QPoint(1,3));
    m_chess += new Chess(this, State::PLAYER3, "3兵", QPoint(4,3));
}

void Board::ini()
{
    for(int i = 0; i < XSize; i++) {
        for(int j = 0; j< YSize; j++) {
            m_grid[i][j] = State::EMPTY;
        }
    }
    m_grid[XSize-1][YSize-1] = State::FORBIDDEN;
}

void Board::paint(QPainter *painter)
{
    QPen pen(m_color, Qt::SolidLine);
    painter->setPen(pen);
    painter->setRenderHints(QPainter::Antialiasing, true);

    int i,j;
    for(i=0; i < XSize-1; i++) {
        for(j=0; j< YSize; j++) {
            if (m_grid[i][j] == State::FORBIDDEN) continue;
            if (m_grid[i+1][j] != State::FORBIDDEN)
                painter->drawLine(i*m_unitLen,j*m_unitLen,(i+1)*m_unitLen,j*m_unitLen);
        }
    }

    for(i=0; i < XSize; i++) {
        for(j=0; j< YSize-1; j++) {
            if (m_grid[i][j] == State::FORBIDDEN) continue;
            if (m_grid[i][j+1] != State::FORBIDDEN)
                painter->drawLine(i*m_unitLen,j*m_unitLen,i*m_unitLen,(j+1)*m_unitLen);
        }
    }
}

void Board::setBoard(QPoint q, State state)
{
    m_grid[q.x()][q.y()] = state;
}

void Board::kill(QPoint q) //kill the chess at q
{
    for(auto it=m_chess.begin(); it!=m_chess.end(); it++)
    {
        if(qobject_cast<Chess*>(*it)->location() == q)
        {
            m_chess.erase(it);
            emit chessListChanged();
            break;
        }
    }
}

Board::State Board::queryBoard(QPoint q)
{
    if (q.x()<0 || q.x()>=XSize || q.y()<0 || q.y()>=YSize )
        return State::FORBIDDEN;
    else
        return m_grid[q.x()][q.y()];
}

void Board::setPredictionMode(const bool mode) {
    m_predictionMode = mode;
    emit predictionModeChanged();
}
