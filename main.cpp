#include "fintpconfigwizard.h"
#include <QApplication>

void setCenterOfApplication(QWidget* widget)
{
    QSize size = widget->sizeHint();
    QDesktopWidget* desktop = QApplication::desktop();
    int width = desktop->width();
    int height = desktop->height();
    widget->move(width/4, height/4);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString fileName;

    if (argc >= 2)
        fileName = QString(argv[1]);
    else
        fileName = QString(argv[0]) + ".xml";
    ConfigUI *configUi = new ConfigUI(fileName);
    configUi->resize(600, 400);
//    configUi->setFixedSize(600,400);
    setCenterOfApplication(configUi);
    configUi->show();

    return a.exec();
}
