#include "chess.h"

Chess::Chess(Board *parent, const Board::State& player, const QString& name, const QPoint& location)
    :QObject(parent), m_player(player), m_name(name), m_location(location), pBoard(parent)  {
    // try out qobject_cast later
    pBoard->setBoard(m_location, m_player);
}

Chess::Chess(Board *parent)
    :Chess::Chess(parent, Board::State::PLAYER1, "兵", QPoint(2,2)) {
}


void Chess::getPrediction()
{
    pBoard->setPredictionMode(true); // enter prediction mode

    const QList<QPoint> rule { QPoint(1,0), QPoint(-1,0), QPoint(0,1), QPoint(0,-1) };
    for (auto i:rule)
    {
        QPoint dest(i + m_location);
        Board::State destState (pBoard->queryBoard(dest));

        if (destState == Board::State::EMPTY ||  // destination is empty or enemy
                destState == Board::State::PLAYER2 || destState == Board::State::PLAYER4)
            m_prediction += new QPointWrapper(dest);
        m_prediction += new QPointWrapper(m_location); // you can cancel selection
    }
    emit predictionChanged();
}

void Chess::moveTo(const QPoint dest)
{
    m_prediction.clear();
    emit predictionChanged();
    pBoard->setPredictionMode(false); // exit prediciton mode


    if (dest == m_location)  // do nothing
        return;

    pBoard->setBoard(m_location, Board::State::EMPTY);
    if (pBoard->queryBoard(dest) != Board::State::EMPTY) // it's a kill
        pBoard->kill(dest);
    pBoard->setBoard(dest, m_player);
    setLocation(dest);
    emit locationChanged();

}


