#include "fintpconfigwizard.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    /*
    FinTPConfigWizard w;
    w.show();
    */
    TabDialog tabdialog;
    tabdialog.show();

    return a.exec();
}
