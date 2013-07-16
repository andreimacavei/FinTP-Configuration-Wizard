#include "fintpconfigwizard.h"
#include "ui_fintpconfigwizard.h"
#include <QtWidgets>

/*
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
*/

TabDialog::TabDialog(QWidget *parent)
    : QDialog(parent)
{

    tabWidget = new QTabWidget;
    tabWidget->addTab(new GeneralTab(), tr("General"));
    tabWidget->addTab(new ConfigurationTab(), tr("Configuration"));
    tabWidget->addTab(new ApplicationsTab(), tr("Applications"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSizeConstraint(QLayout::SetNoConstraint);
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(tr("FinTP Config GUI"));
}

GeneralTab::GeneralTab(QWidget *parent)
    : QWidget(parent)
{
    QLabel *fileNameLabel = new QLabel(tr("File Name"));
    QLineEdit *fileNameEdit = new QLineEdit("Edit file name...");

    QLabel *pathLabel = new QLabel(tr("Path:"));
    QLabel *pathValueLabel = new QLabel("Path/to/file");
    pathValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *sizeLabel = new QLabel(tr("Size:"));
    qlonglong size = 1024;
    QLabel *sizeValueLabel = new QLabel(tr("%1 K").arg(size));
    sizeValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *lastReadLabel = new QLabel(tr("Last Read:"));
    QLabel *lastReadValueLabel = new QLabel("exemplu 2013-07-16");
    lastReadValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *lastModLabel = new QLabel(tr("Last Modified:"));
    QLabel *lastModValueLabel = new QLabel("exemplu 2013-07-16");
    lastModValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(fileNameLabel);
    mainLayout->addWidget(fileNameEdit);
    mainLayout->addWidget(pathLabel);
    mainLayout->addWidget(pathValueLabel);
    mainLayout->addWidget(sizeLabel);
    mainLayout->addWidget(sizeValueLabel);
    mainLayout->addWidget(lastReadLabel);
    mainLayout->addWidget(lastReadValueLabel);
    mainLayout->addWidget(lastModLabel);
    mainLayout->addWidget(lastModValueLabel);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

ConfigurationTab::ConfigurationTab(QWidget *parent)
    : QWidget(parent)
{

}

ApplicationsTab::ApplicationsTab(QWidget *parent)
    :QWidget(parent)
{

}
