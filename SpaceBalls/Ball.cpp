#include "Ball.h"

Ball::Ball()
{
}

bool Ball::IsBall()
{
    bool flag = false;
    for (int i = 0; i < Cap6; i++)
    {
        if (type == i)
            return true;
    }
    return false;
}
