#include "fintpconfigwizard.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString fileName;

    if (argc >= 2)
        fileName = QString(argv[1]);
    else
        fileName = QString(argv[0]) + ".xml";

    ConfigUI configUi(fileName);
    configUi.setFixedSize(600, 400);
    configUi.show();

    return a.exec();
}
