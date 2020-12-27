#include "GameLevel.h"

GameLevel::GameLevel(int level, QuestType questType)
{
    data.level = level;
    data.questType = questType;
}

void GameLevel::UpdateBonusCounter(Ball::Type type)
{
    if (data.bonusType == type)
    {
        ++data.bonusCounter;
        if (data.bonusCounter >= data.bonusCounterMax)
            emit questFinished();
    }
}