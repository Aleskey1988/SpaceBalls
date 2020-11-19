#include "SpaceBalls.h"

SpaceBalls::SpaceBalls(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.beginGroup("main");
    int score = settings.value("score").toInt();
    ui.gameField->SetScore(score);

    connect(ui.buttonTest, &QPushButton::clicked, this, [=]
    {
        ui.gameField->onTest();
    });
    connect(ui.gameField, &GameField::scoreChanged, this, [=] (int value)
    {
        QPixmap pixmap(211, 211);
        QPainter p(&pixmap);
        p.setFont(QFont("Arial", 30));
        p.setPen(QPen(Qt::blue));
        p.drawText(15, 120, QString::number(value));
        ui.labelScore->setPixmap(pixmap);
    });
    emit ui.gameField->scoreChanged(ui.gameField->GetScore());
}
SpaceBalls::~SpaceBalls()
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.beginGroup("main");
    settings.setValue("score", ui.gameField->GetScore());
}