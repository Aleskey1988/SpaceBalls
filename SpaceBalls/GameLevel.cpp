#include "GameLevel.h"

int GameLevel::levelCounter = 1;

GameLevel::GameLevel(QuestType questType, int score)
{
    data.level = levelCounter++;
    data.questType = questType;
    data.scoreMax = score;
}
GameLevel::GameLevel(QuestType questType, int ballCounter, Ball::Type type)
{
    data.level = levelCounter++;
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