#include "SpaceBalls.h"

SpaceBalls::SpaceBalls(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    // load GUI settings
    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.beginGroup("guiSettings");
    QString resolutionStr = settings.value("resolution", "1280x720 (16:9)").toString();
    int fps = settings.value("fps", 60).toInt();
    int soundVolume = settings.value("soundVolume", 75).toInt();
    int musicVolume = settings.value("musicVolume", 75).toInt();

    gameField = new GameField;
    ui.layout->addWidget(gameField);
    // set resolution
    // format: widthxheight (n1:n2)
    QStringList list = resolutionStr.split(" ");
    QStringList list2 = list[0].split("x");
    gameField->SetResoultion(QSize(list2[0].toInt(), list2[1].toInt()));
    gameField->LoadResources();
    gameField->SetFPS(fps);
    gameField->Start();
}
SpaceBalls::~SpaceBalls()
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.beginGroup("userData");
    //settings.setValue("score", gameField->GetScore());

    delete gameField;
}