#include "fintpconfigwizard.h"
#include <QApplication>

/**
 * @brief setCenterOfApplication  Move the passed widget on screen
 * @param widget  the widget to be positioned
 */
void setCenterOfApplication(QWidget* widget)
{
    QSize size = widget->sizeHint();
    QDesktopWidget* desktop = QApplication::desktop();
    int width = desktop->width();
    int height = desktop->height();
    widget->move(width/4, height/5);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString fileName;


    if (argc >= 2)
        fileName = QString(argv[1]);
    else {
        QFileInfo fileInfo(argv[0]);
        QDir dir = fileInfo.absoluteDir();
        dir.cdUp();
        fileName = dir.absolutePath() +"/"+ "FinTP-Configuration-Wizard.xml";
    }
    ConfigUI *configUi = new ConfigUI(fileName);
    configUi->resize(600, 500);
    setCenterOfApplication(configUi);
    configUi->show();

    return a.exec();
}
