#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() & (~Qt::WindowMaximizeButtonHint) | Qt::Window);

    connect(ui.sliderSoundVolume, &QSlider::valueChanged, this, &MainWindow::setSoundVolumeValue);
    connect(ui.sliderMusicVolume, &QSlider::valueChanged, this, &MainWindow::setMusicVolumeValue);

    connect(ui.buttonResetSettings, &QPushButton::clicked, this, [=]
    {
        ui.comboResolution->setCurrentText("1280x720 (16:9)");
        ui.comboFPS->setCurrentText("60");
        ui.sliderSoundVolume->setValue(75);
        ui.sliderMusicVolume->setValue(75);
    });
    connect(ui.buttonResetUserData, &QPushButton::clicked, this, [=]
    {
        QSettings settings("settings.ini", QSettings::IniFormat);
        settings.beginGroup("userData");
        settings.setValue("score", 0);
    });
    connect(ui.buttonBox, &QDialogButtonBox::accepted, this, [=]
    {
        QSettings settings("settings.ini", QSettings::IniFormat);
        settings.beginGroup("guiSettings");
        settings.setValue("resolution", ui.comboResolution->currentText());
        settings.setValue("fps", ui.comboFPS->currentText());
        settings.setValue("soundVolume", ui.sliderSoundVolume->value());
        settings.setValue("musicVolume", ui.sliderMusicVolume->value());
        close();
    });
    connect(ui.buttonBox, &QDialogButtonBox::rejected, this, [=] { close(); });

    // load GUI settings
    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.beginGroup("guiSettings");
    ui.comboResolution->setCurrentText(settings.value("resolution", "1280x720 (16:9)").toString());
    ui.comboFPS->setCurrentText(settings.value("fps", "60").toString());
    ui.sliderSoundVolume->setValue(settings.value("soundVolume", 75).toInt());
    ui.sliderMusicVolume->setValue(settings.value("musicVolume", 75).toInt());
}
