#include "GameLevel.h"

int GameLevel::levelCounter = 1;

GameLevel::GameLevel()
{
    levelNumber = levelCounter++;
}
GameLevel::~GameLevel()
{
    for (auto quest : quests)
        delete quest;
}

GameLevel* GameLevel::AddScoreQuest(const int score)
{
    auto quest = new ScoreQuest(score);
    quests << quest;
    return this;
}
GameLevel* GameLevel::AddBallsQuest(const Ball::Type ballType, const int score)
{
    auto quest = new BallsQuest(ballType, score);
    quests << quest;
    return this;
}
void GameLevel::UpdateQuestData(const int score, const QVector<int>& ballsCounter)
{
    for (auto& quest : quests)
    {
        if (quest->IsScoreQuest())
            quest->UpdateScore(score);
        else if (quest->IsBallsQuest())
        {
            for (int i = 0; i < ballsCounter.size(); ++i)
            {
                if (quest->IsBallsQuest() && dynamic_cast<BallsQuest*>(quest)->GetBallType() == Ball::Type(i))
                    quest->UpdateScore(ballsCounter[i]);
            }
        }
    }
    checkFinished();
}

void GameLevel::checkFinished()
{
    int finishedCounter = 0;
    for (auto& quest : quests)
    {
        if (quest->GetScore() >= quest->GetScoreMax())
            ++finishedCounter;
    }
    if (finishedCounter == quests.size())
        emit questFinished();
}
