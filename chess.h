#ifndef CHESS_H
#define CHESS_H

#include <QObject>
#include <QString>
#include <QPoint>
#include <QList>
#include "board.h"

class Chess : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    //Q_PROPERTY(QPoint location MEMBER m_location NOTIFY locationChanged)
    Q_PROPERTY(QPoint location READ location WRITE setLocation NOTIFY locationChanged)
    Q_PROPERTY(QList<QObject*> prediction MEMBER m_prediction NOTIFY predictionChanged)


public:
    explicit Chess(Board *parent, const Board::State&, const QString&, const QPoint&);
    explicit Chess(Board *parent);
    QString name() const { return m_name; }
    QPoint location() const { return m_location;}
    void setLocation(const QPoint loc) { m_location = loc;}

private:
    const Board::State m_player;
    const QString m_name;
    QPoint m_location;

    Board* const pBoard;
    QList<QObject*> m_prediction;


signals:
    void locationChanged();
    void predictionChanged();


public slots:
    void getPrediction();
    void moveTo(QPoint dest);
};



class QPointWrapper: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QPoint location MEMBER m_point CONSTANT)
public:
    QPointWrapper(QPoint point): m_point(point) {}
private:
    QPoint m_point;
};

#endif // CHESS_H
