#include "fintpconfigwizard.h"

#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>

TabDialog::TabDialog(const QString &fileName, QWidget *parent)
    : QDialog(parent)
{
    tabWidget = new QTabWidget;
    // We load the interface from XML file
    parseXML(fileName);

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

void TabDialog::parseXML(const QString &fileName) {
    QFile* file = new QFile(fileName);
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString error_message = "Couldn't open " + fileName;
        QMessageBox::critical(this,
                              "TabDialog::parseXML",
                              error_message,
                              QMessageBox::Ok);
        return;
    }
    QDomDocument doc("configFile");
    if (!doc.setContent(file))
    {
        QString error = "Error";
        QMessageBox::critical(this,
                              "TabDialog::parseXML",
                              error,
                              QMessageBox::Ok);
        return;
    }

    QDomElement docElem = doc.documentElement();
    QString rootTag = docElem.tagName();

    if(rootTag != "configuration")
    {
        QString error = "XML file with bad format";
        QMessageBox::critical(this,
                              "TabDialog::parseXML",
                              error,
                              QMessageBox::Ok);
    }

    QDomNodeList nodeList = docElem.elementsByTagName("sectionGroup");
    QDomNodeList siblings = docElem.childNodes();

    for(int i = 0; i < siblings.count(); i++)
    {
        //printf("%s\n", siblings.at(i).nodeName().toStdString().c_str());

        if(siblings.at(i).toElement().tagName() == "configSections")
            continue;
        QString tabName = siblings.at(i).toElement().tagName();
        QWidget *tab = new QWidget();
        QFormLayout* layout = new QFormLayout;

        QDomElement el = siblings.at(i).toElement();
        QDomNodeList childList = el.childNodes();

        for(int j = 0; j < childList.count(); j++)
        {
            QDomNode keyEntries = childList.at(j).firstChild();
            while(!keyEntries.isNull()) {
                QDomElement keyData = keyEntries.toElement();
                QString keyName = keyData.attribute("name");
                QString keyAlias = keyData.attribute("alias");
                QString keyList = keyData.attribute("list");
                if(!keyData.text().isEmpty()){
                    QString keyText = keyData.text();
                }

                layout->addRow(keyAlias, new QLineEdit(keyName));

                keyEntries = keyEntries.nextSibling();
            }
        }

        tab->setLayout(layout);
        this->tabWidget->addTab(tab, tabName);
    }
}
