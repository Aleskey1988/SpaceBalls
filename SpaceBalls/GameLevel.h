#pragma once

#include <QObject>

#include "Ball.h"

enum QuestType
{
    Default,
    Score,          // count score
    BallCounter,    // count number of balls of certain type
    BonusCounter,   // count number of bonuses of certain type
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
    int bonusCounter = 0;
    int bonusCounterMax = 0;
    Ball::Type bonusType;
};

class GameLevel : public QObject
{
    Q_OBJECT

public:
    GameLevel() {}
    GameLevel(int level, QuestType questType);
    void SetScoreType(int value) { data.scoreMax = value; }
    void SetBallCounterType(int value, Ball::Type type) { data.ballCounterMax = value; data.ballType = type; }
    void SetBonusCounterType(int value, Ball::Type type) { data.bonusCounterMax = value; data.bonusType = type; }

    bool IsScoreQuest() { return data.questType == QuestType::Score; }
    bool IsBallQuest() { return data.questType == QuestType::BallCounter; }
    bool IsBonusQuest() { return data.questType == QuestType::BonusCounter; }

    void UpdateScore(int value) {}
    void UpdateBallCounter() {}
    void UpdateBonusCounter(Ball::Type type);

    GameLevelData data;

signals:
    void questFinished();
};
