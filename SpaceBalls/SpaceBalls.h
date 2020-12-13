#pragma once

#include <QtWidgets>
#include <QtCore>
#include "ui_SpaceBalls.h"

class SpaceBalls : public QMainWindow
{
    Q_OBJECT

public:
    SpaceBalls(QWidget *parent = Q_NULLPTR);
    ~SpaceBalls();

private:
    Ui::SpaceBallsClass ui;
    int extraBonus1Progress = 0;
    int extraBonus1Max = 10000;
    QRect extraBonus1Rect = QRect(0, 0, 100, 100);
    int adjust = 5;
};
