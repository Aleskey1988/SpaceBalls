#pragma once

#include <QtWidgets>
#include "ui_MainWindow.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);

private:
    void setSoundVolumeValue(int value) { ui.labelSoundVolume->setText(QString::number(value)); }
    void setMusicVolumeValue(int value) { ui.labelMusicVolume->setText(QString::number(value)); }

    Ui::MainWindowClass ui;
};
