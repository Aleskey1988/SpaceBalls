#pragma once

#include <QObject>

#include "Ball.h"

enum QuestType
{
    Default,
    Score,          // count score
    BallCounter,    // count number of balls of certain type
};

struct GameLevelData
{
    int level = 0;
    QuestType questType = QuestType::Default;
    int score = 0;
    int scoreMax = 0;
    int ballCounter = 0;
    int ballCounterMax = 0;
    Ball::Type ballType;
};

class GameLevel : public QObject
{
    Q_OBJECT

public:
    GameLevel() {}
    GameLevel(int level, QuestType questType, int score);
    GameLevel(int level, QuestType questType, int ballCounter, Ball::Type type);

    bool IsScoreQuest() { return data.questType == QuestType::Score; }
    bool IsBallCounterQuest() { return data.questType == QuestType::BallCounter; }

    void UpdateScoreCounter();
    void UpdateBallCounter(Ball::Type type);

    GameLevelData data;

signals:
    void questFinished();
};
