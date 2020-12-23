#include "SpaceBalls.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SpaceBalls w;
    w.show();
    return a.exec();
}
