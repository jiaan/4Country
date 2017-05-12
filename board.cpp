#include "board.h"
#include "chess.h"
#include <QPainter>
#include <QList>
#include "socket.h"


Board::Board(QQuickItem *parent) :QQuickPaintedItem(parent)
{
    for(int i = 0; i < XSize; i++) {
        for(int j = 0; j< YSize; j++) {
            if((i<5 && j <5) || (i<5 && j>13) || (i>13 && j<5) || (i>13 && j>13))
                m_grid[i][j] = State::FORBIDDEN;
            else m_grid[i][j] = State::EMPTY;
        }
    }

}


void Board::paint(QPainter *painter)
{
    QPen pen(Qt::SolidLine);
    painter->setPen(pen);
    painter->setRenderHints(QPainter::Antialiasing, true);

    int i,j;
    for(i=0; i < XSize-1; i++) {
        for(j=0; j< YSize; j++) {
            if (m_grid[i][j] == State::FORBIDDEN) continue;
            if (m_grid[i+1][j] != State::FORBIDDEN)
                painter->drawLine(i*a,j*a,(i+1)*a,j*a);
        }
    }

    for(i=0; i < XSize; i++) {
        for(j=0; j< YSize-1; j++) {
            if (m_grid[i][j] == State::FORBIDDEN) continue;
            if (m_grid[i][j+1] != State::FORBIDDEN)
                painter->drawLine(i*a,j*a,i*a,(j+1)*a);
        }
    }

    //draw palace
    QList<QPoint> palace = {QPoint(9,1), QPoint(1,9), QPoint(17,9), QPoint(9,17)};
    for (auto it:palace ) {
        painter->drawLine((it-QPoint(1,1))*a,(it+QPoint(1,1))*a);
        painter->drawLine((it-QPoint(-1,1))*a,(it+QPoint(-1,1))*a);
    }

    //draw post
    QList<QPoint> post;

    //using rotation to generate all 4 directions
    QList<QPoint> post_offset {QPoint(7,-3),QPoint(7,3)};

    for(i=-4;i<=4;i+=2)
        post_offset+=QPoint(6,i);

    for(auto it:post_offset) {
        post += QPoint(9,9)+QPoint(it.x(),it.y());
        post += QPoint(9,9)+QPoint(it.y(),-it.x());
        post += QPoint(9,9)+QPoint(-it.x(),-it.y());
        post += QPoint(9,9)+QPoint(-it.y(),it.x());
    }

    double sp = 0.1*a; // spacing
    double tl = 0.25*a; // tickle length

    for (auto it:post) {
        for(i=-1;i<=1;i+=2) {
            for(j=-1;j<=1;j+=2) {
                auto p0 = it*a + sp*QPoint(i,j);

                if( (p0.x()<5*a && p0.y()<5*a) || (p0.x()<5*a && p0.y()>13*a)
                        || (p0.x()>13*a && p0.y()<5*a) || (p0.x()>13*a && p0.y()>13*a))
                    continue; //out of border

                painter->drawLine(p0, p0 + tl*QPoint(i,0));
                painter->drawLine(p0, p0 + tl*QPoint(0,j));
            }
        }
    }

}

void Board::setBoard(QPoint q, State state)
{
    m_grid[q.x()][q.y()] = state;
}

void Board::kill(QPoint q) //kill the chess at q
{    
    Chess* p;

     for(auto i = 0; i< m_chess.size(); i++) {
        if(qobject_cast<Chess*>(m_chess[i])->m_location == q) {
            p = qobject_cast<Chess*>(m_chess[i]);
            m_chess.removeAt(i);
        }
    }

    if(p->m_name == "将") {
        auto victim = p->m_player;
        for(auto i = 0; i< m_chess.size(); i++) {
            auto q = qobject_cast<Chess*>(m_chess[i]);
            if(q->m_player == victim ) {
                m_chess.removeAt(i);
                m_grid[q->location().x()][q->location().y()]= State::EMPTY;
            }
        }

        switch (p->m_player) {
        case State::PLAYER1:
            emit playerKilled(m_seat[0]);
            break;
        case State::PLAYER2:
            emit playerKilled(m_seat[1]);
            break;
        case State::PLAYER3:
            emit playerKilled(m_seat[2]);
            break;
        case State::PLAYER4:
            emit playerKilled(m_seat[3]);
            break;
        default:
            break;
        }
    }

    emit chessListChanged();

}

Board::State Board::queryBoard(QPoint q)
{
    if (q.x()<0 || q.x()>=XSize || q.y()<0 || q.y()>=YSize )
        return State::FORBIDDEN;
    else
        return m_grid[q.x()][q.y()];
}


void Board::moveChess_local(int sx, int sy, int dx, int dy) {
    auto player = m_grid[sx][sy];
    auto source = QPoint(sx,sy);
    auto dest = QPoint(dx,dy);
    setBoard(source, Board::State::EMPTY);
    if (queryBoard(dest) != Board::State::EMPTY) // it's a kill
           kill(dest);
    setBoard(dest, player);

    for(auto it:m_chess)
    {
        auto p = qobject_cast<Chess*>(it);
        if(p->location() == source)
        {
            p->setLocation(dest);
            emit p->locationChanged();
            break;
        }
    }
}

void Board::connectSocket(QObject *socket) {
    // connect sockets and slots
    auto ps = qobject_cast<Socket*>(socket);
    connect(ps,SIGNAL(changeLocation(int,int,int,int)),this,SLOT(moveChess(int,int,int,int)));
    connect(this,SIGNAL(requestMove(int,int,int,int)),ps,SLOT(updateLocation(int,int,int,int)));
    connect(this,SIGNAL(playerKilled(uint)),ps,SLOT(removePlayer(uint)));

    // construct look-up table
    for (uint i = 0; i<4; i++)
        m_seat[i]=(ps->mySeat() -1 + i)%4 + 1;

    // initializing chess
    char ids[] = {'P','P','C','M','X','S','J','S','X','M','C','B','B','B','B','B'};
    QList<QPoint> offsets = { QPoint(-3,7), QPoint(3,7) };
    for (int i = -4; i<=4; i++) offsets+=QPoint(i,9);
    for (int i = -4; i<=4; i+=2) offsets+=QPoint(i,6);


    for (int i=0; i<16; i++) {
        m_chess += new Chess(this, State::PLAYER1 ,ids[i], QPoint(9,9)+QPoint(offsets[i].x(),offsets[i].y()));
        m_chess += new Chess(this, State::PLAYER2 ,ids[i], QPoint(9,9)+QPoint(offsets[i].y(),-offsets[i].x()));
        m_chess += new Chess(this, State::PLAYER3 ,ids[i], QPoint(9,9)+QPoint(-offsets[i].x(),-offsets[i].y()));
        m_chess += new Chess(this, State::PLAYER4 ,ids[i], QPoint(9,9)+QPoint(-offsets[i].y(),offsets[i].x()));
    }
    emit chessListChanged();
}

void Board::requestMove_local(int sx, int sy, int dx, int dy) {

    int sx_o = sx-9;
    int sy_o = sy-9;
    int dx_o = dx-9;
    int dy_o = dy-9;
    int sx_u(9), sy_u(9), dx_u(9), dy_u(9); // universal

    switch (m_seat[0]) { // inverse rotation!
    case 1:
        sx_u += sx_o;
        sy_u += sy_o;
        dx_u += dx_o;
        dy_u += dy_o;
        break;
    case 2:
        sx_u += sy_o;
        sy_u += -sx_o;
        dx_u += dy_o;
        dy_u += -dx_o;
        break;
    case 3:
        sx_u += -sx_o;
        sy_u += -sy_o;
        dx_u += -dx_o;
        dy_u += -dy_o;
        break;
    case 4:
        sx_u += -sy_o;
        sy_u += sx_o;
        dx_u += -dy_o;
        dy_u += dx_o;
        break;
    }


    requestMove(sx_u,sy_u,dx_u,dy_u);
}

void Board::moveChess(int sx, int sy, int dx, int dy) {
    int sx_o = sx-9;
    int sy_o = sy-9;
    int dx_o = dx-9;
    int dy_o = dy-9;
    int sx_u(9), sy_u(9), dx_u(9), dy_u(9);

    switch (m_seat[0]) { // rotation!
    case 1:
        sx_u += sx_o;
        sy_u += sy_o;
        dx_u += dx_o;
        dy_u += dy_o;
        break;
    case 2:
        sx_u += -sy_o;
        sy_u += sx_o;
        dx_u += -dy_o;
        dy_u += dx_o;
        break;
    case 3:
        sx_u += -sx_o;
        sy_u += -sy_o;
        dx_u += -dx_o;
        dy_u += -dy_o;
        break;
    case 4:
        sx_u += sy_o;
        sy_u += -sx_o;
        dx_u += dy_o;
        dy_u += -dx_o;
        break;
    }

    moveChess_local(sx_u,sy_u,dx_u,dy_u);
}
