#ifndef BOARD_H
#define BOARD_H

#include <QtQuick/QQuickPaintedItem>
#include <QPoint>
#include <QColor>


class Board : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(int unitLen MEMBER m_unitLen NOTIFY unitLenChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QList<QObject*> chessList MEMBER m_chess NOTIFY chessListChanged)
    Q_PROPERTY(bool predictionMode READ predictionMode WRITE setPredictionMode NOTIFY predictionModeChanged)

public:
    Board(QQuickItem *parent = 0);
    enum class State { FORBIDDEN, EMPTY, PLAYER1, PLAYER2, PLAYER3, PLAYER4};
    void paint(QPainter *painter);

    QColor color() const { return m_color;}
    void setColor(const QColor &color) { m_color = color;}
    bool predictionMode() const { return m_predictionMode; }
    void setPredictionMode(const bool mode);

    State queryBoard(QPoint q);
    void setBoard(QPoint q, State state);
    QList<QObject*> m_chess;
    void kill(QPoint q);


private:
    static const int XSize = 10;
    static const int YSize = 5;
    State m_grid[XSize][YSize];
    int m_unitLen;
    bool m_predictionMode = false;
    QColor m_color;
    void ini();

signals:
    void colorChanged();
    void unitLenChanged();
    void chessListChanged();
    void predictionModeChanged();

public slots:
    int getWidth() const { return XSize;}
    int getHeight() const { return YSize;}

};

#endif // BOARD_H
