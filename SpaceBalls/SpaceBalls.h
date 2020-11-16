#pragma once

#include <QtWidgets/QMainWindow>
#include <QPainter>
#include "ui_SpaceBalls.h"

class SpaceBalls : public QMainWindow
{
    Q_OBJECT

public:
    SpaceBalls(QWidget *parent = Q_NULLPTR);

private:
    Ui::SpaceBallsClass ui;
};
