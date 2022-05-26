#pragma once

#include <QObject>

#include "Ball.h"

enum QuestType
{
    Default,
    Score,          // count score
    Balls,          // count number of balls of certain type
};

class Quest
{
public:
    Quest(const QuestType questType, const int scoreMax)
        : questType(questType), scoreMax(scoreMax)
    {
    }
    virtual ~Quest() = 0 {};

    bool IsScoreQuest() const { return questType == QuestType::Score; }
    bool IsBallsQuest() const { return questType == QuestType::Balls; }

    int GetScore() const { return score; };
    int GetScoreMax() const { return scoreMax; };

    void UpdateScore(const int value) { score += value; if (score > scoreMax) score = scoreMax; };

protected:
    QuestType questType;
    int score = 0;
    int scoreMax = 0;
};

class ScoreQuest : public Quest
{
public:
    ScoreQuest(const int scoreMax)
        : Quest(QuestType::Score, scoreMax)
    {
    }
    virtual ~ScoreQuest() {}
};

class BallsQuest : public Quest
{
public:
    BallsQuest(const Ball::Type ballType, const int scoreMax)
        : Quest(QuestType::Balls, scoreMax), ballType(ballType)
    {
    }
    virtual ~BallsQuest() {}

    Ball::Type GetBallType() const { return ballType; }

private:
    Ball::Type ballType;
};

class GameLevel : public QObject
{
    Q_OBJECT

public:
    GameLevel();
    ~GameLevel();

    int GetLevelNumber() const { return levelNumber; }
    void AddScore(const int value) { score += value; };
    int GetScore() const { return score; }

    GameLevel* AddScoreQuest(const int score);
    GameLevel* AddBallsQuest(const Ball::Type ballType, const int ballCounter);
    QList<Quest*> GetQuests() const { return quests; }
    void UpdateQuestData(const int score, const QVector<int>& ballsCounter);

signals:
    void questFinished();

private:
    void checkFinished();

    static int levelCounter;
    int levelNumber = 0;
    QList<Quest*> quests;
    int score = 0;

};
