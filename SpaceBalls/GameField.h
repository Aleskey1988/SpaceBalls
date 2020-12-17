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
    void SetScore(int value) { score = value; }
    int GetScore() { return score; }

protected:
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseDoubleClickEvent(QMouseEvent* e);

private:
    enum class MoveType
    {
        Ball,
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
        Ball,
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
    void shuffleBalls();
    QPoint getRandomBallPos();
    void swapBalls(int x, int y);

    Ui::GameField ui;
    QVector<QVector<Ball>> balls;
    QVector<QVector<ChainBall>> chainBalls;

    QSize fieldSize = QSize(12, 8);
    int ballSize = 51;
    double ballGapPercent = 0.07;
    int ballGap = ballSize * ballGapPercent;
    QSize gameFieldSize = QSize(1000, 700);
    QRect gameFieldArea = QRect(200, 25, fieldSize.width() * ballSize, fieldSize.height() * ballSize);
    QRect scoreArea = QRect(25, 25, 150, 100);
    QRect extraBonusesArea = QRect(850, 25, 125, 400);
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

    struct Bonus4
    {
        bool isActive = false;
        QRect rect;
        QPen pen = QPen(QColor(255, 192, 0), 4);
        QBrush brush = QBrush(Qt::transparent);
    } b4;

    bool isBonus5 = false;
    QRect bonus5Rect;
    double bonus5Transparent = 0.0;

    struct Bonus6
    {
        bool isStage1 = false;
        QRect rect;
        double angle = 0;
    } b6;

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
        double progress = 0;
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
    int fps = 60;
    int score = 0;
    int prevScore = 0;

    bool isUseMouse = true;

    struct ExtraBonusProgress
    {
        int value = 0;
        int maxValue = 0;
        QRect rect;
        QPoint pos;
    };
    ExtraBonusProgress ebp1;
    ExtraBonusProgress ebp2;
    ExtraBonusProgress ebp3;
    int adjust = 5;
};
