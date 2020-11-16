#include "SpaceBalls.h"

SpaceBalls::SpaceBalls(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    connect(ui.buttonTest, &QPushButton::clicked, this, [=]
    {
        ui.gameField->onTest();
    });
    connect(ui.gameField, &GameField::scoreChanged, this, [=] (int value)
    {
        QPixmap pixmap(211, 211);
        QPainter p(&pixmap);
        p.setFont(QFont("Arial", 60));
        p.setPen(QPen(Qt::blue));
        p.drawText(15, 135, QString::number(value));
        ui.labelScore->setPixmap(pixmap);
    });
    emit ui.gameField->scoreChanged(0);
}
