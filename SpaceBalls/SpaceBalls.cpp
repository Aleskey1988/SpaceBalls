#include "SpaceBalls.h"

SpaceBalls::SpaceBalls(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.beginGroup("main");
    //int score = settings.value("score").toInt();
    //ui.gameField->SetScore(score);

    connect(ui.buttonTest, &QPushButton::clicked, this, [=]
    {
        ui.gameField->Test();
    });
}
SpaceBalls::~SpaceBalls()
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.beginGroup("main");
    settings.setValue("score", ui.gameField->GetScore());
}