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
    struct ChainBall
    {
        Ball::Type type;
        bool checked = false;
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
    
    void updateGameField();
    QList<QPoint> getShape(int x, int y);
    QList<QList<QPoint>> getShapes();
    QList<QList<QPoint>> getLineShapes(QList<QList<QPoint>>& shapes);
    void removeBalls(QList<QList<QPoint>>& shapes);
    QList<QPair<QPoint, QPoint>> getDropData();
    QImage SvgToImage(QString& fileName);
    
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
    QList<QSound*> sounds;
    QImage selectedImage;

    int timerTick = 5;
    int score = 0;
};
