#pragma once

#include <QtWidgets>
#include <QtMultimedia>
#include <QtSvg>

#include "ui_GameField.h"
#include "Ball.h"

class GameField : public QLabel
{
    Q_OBJECT

public:
    GameField(QWidget *parent = Q_NULLPTR);

    void onTest();
    void SetScore(int value) { score = value; }
    int GetScore() { return score; }

signals:
    void scoreChanged(int value);

protected:
    void mousePressEvent(QMouseEvent* e);
    void mouseDoubleClickEvent(QMouseEvent* e);

private:
    enum MoveType
    {
        Cap,
        Bonus,
    };
    struct ChainBall
    {
        Ball::Type type;
        bool checked = false;
    };
    struct PossibleMove
    {
        PossibleMove(MoveType type, QPoint p1, QPoint p2, int length)
            : type(type), p1(p1), p2(p2), length(length) {}
        MoveType type;
        QPoint p1;
        QPoint p2;
        int length = 0;
    };
    enum Sound
    {
        RemoveBalls,
        WrongMove,
        UseBonus4,
        UseBonus5,
        UseBonus6,
        UseBonusExtra1,
        UseBonusExtra2,
        UseBonusExtra3,
    };
    enum class RemoveType
    {
        Cap,
        Bonus,
    };
    
    void updateGameField();
    QList<QPoint> getShape(int x, int y);
    QList<QList<QPoint>> getShapes(QVector<QVector<Ball>>& balls);
    QList<QList<QPoint>> getLineShapes(QList<QList<QPoint>>& shapes);
    void removeBalls(QList<QList<QPoint>>& shapes, RemoveType removeType);
    QList<QPair<QPoint, QPoint>> getDropData();
    QImage SvgToImage(QString& fileName);
    QList<PossibleMove> getPossibleMoves();
    void shuffleCaps();
    
    Ui::GameField ui;
    QVector<QVector<Ball>> balls;
    QVector<QVector<ChainBall>> chainBalls;

    QSize fieldSize = QSize(12, 8);
    QSize ballSize = QSize(70, 70);
    int ballGap = 5;
    bool isFirstSelected = false;
    Ball* firstBall = nullptr;
    Ball* secondBall = nullptr;

    QTimer timer;
    QTimer ballsSwapTimer;
    QTimer removeTimer;
    QTimer dropTimer;
    QTimer fillTimer;

    int swapCounter = 0;
    int removeCounter = 0;
    int fillCounter = 0;
    int dropCounter = 0;

    QImage background;
    QList<QImage> textures;
    QList<QImage> extraBonus2Textures;
    QList<QSound*> sounds;
    QImage selectedImage;

    bool isExtraBonus2Pos = false;
    bool isExtraBonus2Lines = false;
    QList<QPoint> extraBonus2Pos;
    QList<QLine> extraBonus2Lines;
    QPen extraBonus2Pen;
    double extraBonus2Transparent = 0.0;

    int timerTick = 5;
    int score = 0;
};
