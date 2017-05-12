#ifndef BOARD_H
#define BOARD_H

#include <QtQuick/QQuickPaintedItem>
#include <QPoint>
#include <QColor>

class Board : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(int a MEMBER a NOTIFY aChanged)
    Q_PROPERTY(int length READ length CONSTANT)
    Q_PROPERTY(QList<QObject*> chessList MEMBER m_chess NOTIFY chessListChanged)

public:
    Board(QQuickItem *parent = 0);
    enum class State { FORBIDDEN, EMPTY, PLAYER1, PLAYER2, PLAYER3, PLAYER4};
    void paint(QPainter *painter);


    State queryBoard(QPoint q);
    void setBoard(QPoint q, State state);
    QList<QObject*> m_chess;
    void kill(QPoint q);


private:
    static const int XSize = 19;
    static const int YSize = 19;
    State m_grid[XSize][YSize];

    int a; // unit length
    QColor m_color;
    uint m_seat[4]; //seat look-up table


public:
    int length() const { return (XSize-1)*a;} // total length
    void requestMove_local(int sx, int sy, int dx, int dy);
    void moveChess_local(int sx, int sy, int dx, int dy);
    uint seat() { return m_seat[0]; }

signals:
    void aChanged();
    void chessListChanged();
    void requestMove(int sx, int sy, int dx, int dy);
    void playerKilled(uint i);



public slots:
    void moveChess(int sx, int sy, int dx, int dy);
    void connectSocket(QObject* socket);


};

#endif // BOARD_H
