#include "fintpconfigwizard.h"
#include "ui_fintpconfigwizard.h"

FinTPConfigWizard::FinTPConfigWizard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FinTPConfigWizard)
{
    ui->setupUi(this);
}

FinTPConfigWizard::~FinTPConfigWizard()
{
    delete ui;
}
