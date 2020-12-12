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

    void Test();
    void CallExtraBonus1();
    void CallExtraBonus2();
    void CallExtraBonus3();
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
            : type(type), p1(p1), p2(p2), length(length)
        {
        }
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
    QPoint getRandomCapPos();

    Ui::GameField ui;
    QVector<QVector<Ball>> balls;
    QVector<QVector<ChainBall>> chainBalls;

    QSize fieldSize = QSize(12, 8);
    int ballSize = 70;
    double ballGapPercent = 0.07;
    int ballGap = ballSize * ballGapPercent;
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
    QList<QImage> extraBonus1Textures;
    QList<QImage> extraBonus2Textures;
    QList<QSound*> sounds;
    QImage selectedImage;

    bool isBonus4 = false;
    QRect bonus4Rect;
    QPen bonus4Pen;

    bool isBonus5 = false;
    QRect bonus5Rect;
    double bonus5Transparent = 0.0;

    struct ExtraBonus1
    {
        bool isStage1 = false;
        bool isStage2 = false;
        QList<QPointF> startPoints;
        QList<QPointF> endPoints;
        QList<QPointF> stage1CurPoints;
        QList<int> meteorType;
        QList<double> angles;
        QVector<QList<QPointF>> stage2CurPoints;
        double opacity = 0.0;
        int numMeteors = 5;
    } eb1;

    bool isExtraBonus2Pos = false;
    bool isExtraBonus2Lines = false;
    QList<QPoint> extraBonus2Pos;
    QList<QLine> extraBonus2Lines;
    QPen extraBonus2Pen;
    double extraBonus2Transparent = 0.0;

    struct ExtraBonus3
    {
        bool isStage1 = false;
        bool isStage2 = false;
        QList<QLine> lines;
        QPainterPath path;
        QPen pen;
        QBrush brush;
        double transparent = 0.0;
    } eb3;

    int timerTick = 5;
    int score = 0;

    bool isUseMouse = true;
};
