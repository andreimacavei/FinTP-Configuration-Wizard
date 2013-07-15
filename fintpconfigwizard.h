#ifndef FINTPCONFIGWIZARD_H
#define FINTPCONFIGWIZARD_H

#include <QMainWindow>

namespace Ui {
class FinTPConfigWizard;
}

class FinTPConfigWizard : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit FinTPConfigWizard(QWidget *parent = 0);
    ~FinTPConfigWizard();
    
private:
    Ui::FinTPConfigWizard *ui;
};

#endif // FINTPCONFIGWIZARD_H
