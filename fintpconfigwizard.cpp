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
    tabWidget->addTab(new GeneralTab(), tr("Fetcher"));
    tabWidget->addTab(new ConfigurationTab(), tr("Publisher"));
    tabWidget->addTab(new ApplicationsTab(), tr("Watcher"));

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
    QLabel *fileNameLabel = new QLabel(tr("qPIFilter_0"));
    QLineEdit *fileNameEdit = new QLineEdit("Enter file to be fetched...");

    QLabel *pathLabel = new QLabel(tr("qPIFilter_1"));
    QLabel *pathValueLabel = new QLabel("Path/to/file");
    pathValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
/*
    QLabel *sizeLabel = new QLabel(tr("qPIFilter_2"));
    qlonglong size = 1024;
    QLabel *sizeValueLabel = new QLabel(tr("%1 K").arg(size));
    sizeValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
*/
    QLabel *lastReadLabel = new QLabel(tr("qPIFilter_3"));
    QLabel *lastReadValueLabel = new QLabel("XSLT");
    lastReadValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *lastModLabel = new QLabel(tr("qPIFilter_3"));
    QLabel *lastModValueLabel = new QLabel("WMQ");
    lastModValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(fileNameLabel);
    mainLayout->addWidget(fileNameEdit);
    mainLayout->addWidget(pathLabel);
    mainLayout->addWidget(pathValueLabel);
//    mainLayout->addWidget(sizeLabel);
//    mainLayout->addWidget(sizeValueLabel);
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
