#pragma once

#include <QtWidgets>
#include <QtMultimedia>
#include <QtSvg>

#include "ui_GameField.h"
#include "Ball.h"
#include "GameLevel.h"

class GameField : public QLabel
{
    Q_OBJECT

public:
    GameField(QWidget *parent = Q_NULLPTR);
    ~GameField();

    void SetGameLevel(int value) { level.data = levels[value - 1]->data; }
    int GetGameLevel() { return level.data.level; }

    void SetResolution(QSize& size);
    void SetFPS(int value) { fps = value; }
    void SetSoundVolume(int value) { soundVolume = value; }
    void SetMusicVolume(int value) { musicVolume = value; }
    void LoadResources();
    void Start();

signals:
    void fpsChanged(double value);

protected:
    void paintEvent(QPaintEvent* e);
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
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

    void updateStartScreen();
    void updateGameField();
    QList<QPoint> getShape(int x, int y);
    QList<QList<QPoint>> getShapes(QVector<QVector<Ball>>& balls);
    QList<QList<QPoint>> getLineShapes(QList<QList<QPoint>>& shapes);
    void removeBalls(QList<QList<QPoint>>& shapes, RemoveType removeType);
    QList<QPair<QPoint, QPoint>> getDropData();
    QPixmap SvgToImage(QString& fileName);
    QList<PossibleMove> getPossibleMoves();
    void shuffleBalls();
    QPoint getRandomBallPos();
    void swapBalls(int x, int y);
    void startNextLevel();

    Ui::GameField ui;
    QVector<QVector<Ball>> balls;
    QVector<QVector<ChainBall>> chainBalls;

    QSize fieldSize = QSize(9, 9);
    int ballSize = 61;
    double ballGapPercent = 0.07;
    int ballGap = ballSize * ballGapPercent;

    QSize gameFieldSize;
    int areaGap;
    QRect levelRect;
    QRect questRect;
    QRect extraBonusesRect;
    QRect gameFieldRect;

    QSize logoSize;
    QRect logoRect;
    QSize playButtonSize;
    QRect playButtonRect;
    double startScreenBackgroundOpacity = 1;
    double gameFieldBackgroundOpacity = 0;
    double logoAndPlayButtonOpacity = 1;
    bool drawGameFieldRect = false;
    bool drawBalls = false;

    bool isFirstSelected = false;
    Ball* firstBall = nullptr;
    Ball* secondBall = nullptr;

    QPen framePen = QPen(QColor(255, 255, 255, 160), 4);
    QBrush frameBrush = QBrush(QColor(160, 160, 255, 64));

    QTimer updateStartScreenTimer;
    QTimer updateGameFieldTimer;
    QTimer ballsSwapTimer;
    QTimer removeTimer;
    QTimer dropTimer;
    QTimer fillTimer;
    QTimer timerOne;
    QTimer timerTwo;
    QTimer timerThree;

    int fps = 60;
    int timerTick = 5;
    QElapsedTimer fpsTimer;
    QTimer updateFpsTimer;
    int fpsElapsed = 0;
    double realFps = 0;

    int swapCounter = 0;
    int removeCounter = 0;
    int fillCounter = 0;
    int dropCounter = 0;
    int timerOneCounter = 0;
    int timerTwoCounter = 0;
    int timerThreeCounter = 0;

    int prevScore = 0;
    bool isUseMouse = true;

    QPixmap startScreenBackground;
    QPixmap gameFieldBackground;
    QList<QPixmap> textures;
    QList<QPixmap> extraBonus1Textures;
    QList<QPixmap> extraBonus2Textures;
    QList<QSound*> sounds;
    QPixmap selection;

    struct Bonus4
    {
        bool isActive = false;
        QRect rect;
        QPen pen = QPen(QColor(255, 192, 0), 4);
        QBrush brush = QBrush(Qt::transparent);
    } b4;

    struct Bonus5
    {
        bool isActive = false;
        QRect rect;
        double opacity = 0.0;
    } b5;

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
        QList<QList<int>> meteorsType;
        QList<double> angles;
        QVector<QList<QPointF>> stage2CurPoints;
        double opacity = 0.0;
        int numMeteors = 7;
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

    enum class GameMode
    {
        StartScreen,
        Game,
    };

    GameMode gameMode = GameMode::StartScreen;

    QScopedPointer<QMediaPlaylist> playlist;
    QScopedPointer<QMediaPlayer> player;

    enum Music
    {
        StartScreen,
        Level1,
        Level2,
    };

    int soundVolume = 0;
    int musicVolume = 0;

    QList<GameLevel*> levels;
    GameLevel level;

    bool isStartNextLevel = false;
};
