#include "fintpconfigwizard.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString fileName;
    QString filePath = QCoreApplication::applicationFilePath();
    QFileInfo fileInfo(filePath);

    /*
    FinTPConfigWizard w;
    w.show();
    */

    if (argc >= 2)
        fileName = "../FinTP-Configuration-Wizard/" + QString(argv[1]);
    else
        fileName = "../FinTP-Configuration-Wizard/" + fileInfo.fileName() + ".xml";

    TabDialog tabdialog(fileName);
    tabdialog.show();

    return a.exec();
}
