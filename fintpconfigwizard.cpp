#include "fintpconfigwizard.h"

#include <QDomElement>
#include <QDomNodeList>
#include <QTextStream>

ConfigUI::ConfigUI(const QString &fileName, QWidget *parent)
    : QMainWindow(parent)
{
    tabWidget = new QTabWidget;
    // We load the interface from XML file

    parseXML(fileName);
    /*
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    */
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Open"), this, SLOT(openFile()), QKeySequence::Open);
    fileMenu->addAction(tr("&Save"), this, SLOT(updateFile()), QKeySequence::Save);
    fileMenu->addAction(tr("Save &As"), this, SLOT(saveFile()), QKeySequence::SaveAs);
    fileMenu->addAction(tr("E&xit"), this, SLOT(close()), QKeySequence::Quit);

/*
    QPushButton* saveButton = new QPushButton("Save");
    connect(saveButton, SIGNAL(clicked()),this, SLOT(accept()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
*/

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(tabWidget);
    scrollArea->setWidgetResizable(true);

/*
    mainLayout->setSizeConstraint(QLayout::SetNoConstraint);
    mainLayout->addWidget(scrollArea);
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
*/

    setCentralWidget(scrollArea);

    setWindowTitle(tr("FinTP Config GUI"));
}

void ConfigUI::saveFile()
{
    QDomDocument docDocument("XmlConfigUI");
    QDomElement root = docDocument.documentElement();
    QDomProcessingInstruction instr = docDocument.createProcessingInstruction(
                        "xml", "version='1.0' encoding='UTF-8'");
    QDomElement configSetElement = docDocument.createElement("configuration");
    docDocument.appendChild(instr);
    root.appendChild(configSetElement);
    docDocument.appendChild(root);

    for(int ii = 0; ii < tabWidget->count(); ++ii)
    {
        QWidget *tab = tabWidget->widget(ii);
        QList<QLineEdit *> allLineEdits = tab->findChildren<QLineEdit *>();


        if(allLineEdits.count() > 0)
        {
            for(int jj = 0; jj < allLineEdits.count(); ++jj)
            {
                QDomText newText = docDocument.createTextNode(allLineEdits[jj]->text());
                QDomElement newNodeTag = docDocument.createElement(QString("key"));
                //newNodeTag.setAttribute("alias", allLineEdits[jj]->text());
                newNodeTag.appendChild(newText);
                docDocument.appendChild(newNodeTag);
            }
        }
    }
    QString fileName = "output.xml";
    QFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&file);
        docDocument.save(out, 4);
    }
}

void ConfigUI::updateFile()
{

}

void ConfigUI::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",
                            tr("Text Files (*.txt);;C++ Files (*.cpp *.h);;Xml Files (*.xml *.xslt);;All Files (*.*)"));
    if (fileName != "") {
        ConfigUI::parseXML(fileName);
        /*QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
            return;
        }
        QTextStream in(&file);
        textEdit->setText(in.readAll());
        file.close();
        */
     }

}

QDomElement ConfigUI::addElement( QDomDocument &doc, QDomNode &node,
                        const QString &tag,
                        const QString &value = QString::null )
{
    QDomElement el = doc.createElement( tag );
    node.appendChild( el );
    if ( !value.isNull() ) {
        QDomText txt = doc.createTextNode( value );
        el.appendChild( txt );
    }
    return el;
}

void ConfigUI::parseXML(const QString &fileName) {
    QFile* file = new QFile(fileName);
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString error_message = "Couldn't open " + fileName;
        QMessageBox::critical(this,
                              "TabDialog::parseXML",
                              error_message,
                              QMessageBox::Ok);
        return;
    }
    //QDomDocument doc("configFile");
    if (!this->doc.setContent(file))
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
                QString keyText = keyData.text();

                if(!keyData.attribute("list").isEmpty())
                {
                    QString keyList = keyData.attribute("list");
                    QStringList keyValues = keyList.split(',');
                    QComboBox *comboBox = new QComboBox();
                    comboBox->addItems(keyValues);
                    layout->addRow(keyAlias, comboBox);
                }
                else
                {
                    layout->addRow(keyAlias, new QLineEdit(keyText));
                }
                keyEntries = keyEntries.nextSibling();
            }
        }

        tab->setLayout(layout);
        this->tabWidget->addTab(tab, tabName);
    }
}
