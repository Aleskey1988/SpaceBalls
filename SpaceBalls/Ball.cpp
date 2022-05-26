#include "Ball.h"

Ball::Ball()
{
}

bool Ball::IsBall()
{
    bool flag = false;
    for (int i = Ball1; i <= Ball6; i++)
    {
        if (type == i)
            return true;
    }
    return false;
}
