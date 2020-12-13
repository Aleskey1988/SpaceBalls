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
    connect(ui.buttonCallExtraBonus1, &QPushButton::clicked, this, [=]
    {
        extraBonus1Progress = 0;
        ui.gameField->CallExtraBonus1();
    });
    connect(ui.buttonCallExtraBonus2, &QPushButton::clicked, this, [=]
    {
        ui.gameField->CallExtraBonus2();
    });
    connect(ui.buttonCallExtraBonus3, &QPushButton::clicked, this, [=]
    {
        ui.gameField->CallExtraBonus3();
    });
    connect(ui.gameField, &GameField::scoreChanged, this, [=] (int value, int prevValue)
    {
        QPixmap scorePixmap(211, 211);
        scorePixmap.fill(Qt::transparent);
        QPainter p(&scorePixmap);
        p.setFont(QFont("Arial", 30));
        p.setPen(QPen(Qt::blue));
        p.drawText(15, 120, QString::number(value));
        ui.labelScore->setPixmap(scorePixmap);
        // 
        int delta = value - prevValue;
        extraBonus1Progress += delta;
        if (extraBonus1Progress >= extraBonus1Max)
        {
            extraBonus1Progress = extraBonus1Max;
        }
        QPixmap extraBonus1pixmap(extraBonus1Rect.size());
        extraBonus1pixmap.fill(Qt::transparent);
        p.end();
        p.begin(&extraBonus1pixmap);
        p.setRenderHint(QPainter::Antialiasing);
        QPainterPath path;
        double progress = (double)extraBonus1Progress / extraBonus1Max * 360;
        // bonus is not filled; draw pie
        p.setPen(QPen(Qt::transparent));
        p.setBrush(QBrush(QColor(192, 192, 255)));
        if (progress < 360.0)
        {
            path.moveTo(extraBonus1pixmap.rect().adjusted(adjust, adjust, -adjust, -adjust).center());
            path.arcTo(extraBonus1Rect.adjusted(adjust, adjust, -adjust, -adjust), -90, -progress);
            path.closeSubpath();
            p.drawPath(path);
        }
        // bonus is filled; draw ellipse
        else
        {
            p.drawEllipse(extraBonus1Rect.adjusted(adjust, adjust, -adjust, -adjust));
        }
        // draw outer bonus image circle
        p.setPen(QPen(QColor(64, 64, 255), 2));
        p.setBrush(QBrush(Qt::transparent));
        p.drawEllipse(extraBonus1Rect.adjusted(adjust, adjust, -adjust, -adjust));
        // draw bonus image background
        p.setBrush(QBrush(QColor(128, 128, 255)));
        p.drawEllipse(extraBonus1Rect.adjusted(adjust * 3, adjust * 3, -adjust * 3, -adjust * 3));
        // draw bonus image
        QSvgRenderer r(QString(":/bonuses/Resources/bonuses/extra-1.svg"));
        QImage image(50, 50, QImage::Format_ARGB32);
        image.fill(Qt::transparent);
        r.render(&p, QRectF(25, 25, 50, 50));
        ui.buttonCallExtraBonus1->setIcon(QIcon(extraBonus1pixmap));
        p.end();
    });
    emit ui.gameField->scoreChanged(ui.gameField->GetScore(), 0);
}
SpaceBalls::~SpaceBalls()
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.beginGroup("main");
    settings.setValue("score", ui.gameField->GetScore());
}