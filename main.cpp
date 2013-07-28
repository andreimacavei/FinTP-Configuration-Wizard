#include "fintpconfigwizard.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString fileName;
    /*
    FinTPConfigWizard w;
    w.show();
    */

    if (argc >= 2)
        fileName = argv[1];
    else
        fileName = "../FinTP-Configuration-Wizard/example2.xml";
    TabDialog tabdialog(fileName);
    tabdialog.show();

    return a.exec();
}
