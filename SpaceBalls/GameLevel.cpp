#include "GameLevel.h"

GameLevel::GameLevel(int level, QuestType questType, int score)
{
    data.level = level;
    data.questType = questType;
    data.scoreMax = score;
}
GameLevel::GameLevel(int level, QuestType questType, int ballCounter, Ball::Type type)
{
    data.level = level;
    data.questType = questType;
    data.ballCounterMax = ballCounter;
    data.ballType = type;
}

void GameLevel::UpdateScoreCounter()
{
    if (data.score >= data.scoreMax)
        emit questFinished();
}
void GameLevel::UpdateBallCounter(Ball::Type type)
{
    if (data.ballType == type)
    {
        ++data.ballCounter;
        if (data.ballCounter >= data.ballCounterMax)
            emit questFinished();
    }
}