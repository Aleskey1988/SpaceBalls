#pragma once

#include <QRect>

class Ball
{
public:
    enum Type
    {
        Cap1,
        Cap2,
        Cap3,
        Cap4,
        Cap5,
        Cap6,
        Empty,
        Bonus4,
        Bonus5,
        Bonus6,
        ExtraBonus1,    // meteors
        ExtraBonus2,    // rockets
        ExtraBonus3,    // constellation
    };

    Ball();

    void SetRect(QRect rect) { this->rect = rect; }
    QRect GetRect() { return rect; }

    void SetType(Type type) { this->type = type; }
    Type GetType() { return type; }

    void SetPos(QPoint pos) { this->pos = pos; }
    QPoint GetPos() { return pos; }

    void SetSelected(bool flag) { isSelected = flag; }
    bool GetSelected() { return isSelected; }

    static int GetBallNumber() { return 4; }

    bool IsBall();

private:
    QRect rect;
    Type type;
    QPoint pos;
    bool isSelected = false;
};
