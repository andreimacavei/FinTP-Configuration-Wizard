#include "fintpconfigwizard.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FinTPConfigWizard w;
    w.show();
    
    return a.exec();
}
