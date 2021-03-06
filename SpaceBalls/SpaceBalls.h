#pragma once

#include <QtWidgets>
#include <QtCore>
#include "ui_SpaceBalls.h"
#include "GameField.h"

class SpaceBalls : public QMainWindow
{
    Q_OBJECT

public:
    SpaceBalls(QWidget *parent = Q_NULLPTR);
    ~SpaceBalls();

private:
    Ui::SpaceBallsClass ui;
    GameField* gameField = nullptr;
};
