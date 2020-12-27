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
    settings.endGroup();
    // load user settings
    settings.beginGroup("userData");
    int level = settings.value("level", 1).toInt();

    gameField = new GameField;
    ui.layout->addWidget(gameField);
    gameField->SetGameLevel(level);
    // set resolution
    // format: widthxheight (n1:n2)
    QStringList list = resolutionStr.split(" ");
    QStringList list2 = list[0].split("x");
    gameField->SetResolution(QSize(list2[0].toInt(), list2[1].toInt()));
    gameField->LoadResources();
    gameField->SetFPS(fps);
    gameField->Start();

    connect(gameField, &GameField::fpsChanged, this, [=] (double value)
    {
        setWindowTitle("Space Balls (fps: " + QString::number(value, 'f', 0) + ")");
    });
}
SpaceBalls::~SpaceBalls()
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.beginGroup("userData");
    settings.setValue("level", gameField->GetGameLevel());

    delete gameField;
}