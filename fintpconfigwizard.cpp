#include "fintpconfigwizard.h"

#include <QDomElement>
#include <QDomNodeList>
#include <QTextStream>

ConfigUI::ConfigUI(const QString &fileName, QWidget *parent)
    : QMainWindow(parent)
{
    tabWidget = new QTabWidget;
    QFile* file = new QFile(fileName);
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString error_message = "Couldn't open " + fileName;
        QMessageBox::critical(this, "TabDialog::parseXML", error_message, QMessageBox::Ok);
        return;
    }
    if (!this->doc.setContent(file)){
        QMessageBox::critical(this, "TabDialog::parseXML", "Unable to set DOM parser", QMessageBox::Ok);
        return;
    }
    parseXML(doc);

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Open"), this, SLOT(openFile()), QKeySequence::Open);
    fileMenu->addAction(tr("&Save"), this, SLOT(saveFile()), QKeySequence::Save);
    fileMenu->addAction(tr("Save &As"), this, SLOT(saveFileAs()), QKeySequence::SaveAs);
    fileMenu->addAction(tr("E&xit"), this, SLOT(close()), QKeySequence::Quit);

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(tabWidget);
    scrollArea->setWidgetResizable(true);

    setCentralWidget(scrollArea);
    setWindowTitle(tr("FinTP Config GUI"));
}

void ConfigUI::saveFile()
{
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

void ConfigUI::saveFileAs()
{
    QDomDocument docDocument;
    QDomProcessingInstruction instr = docDocument.createProcessingInstruction(
                        "xml", "version='1.0' encoding='UTF-8'");
    docDocument.appendChild(instr);
    QDomElement configElem = docDocument.createElement("configuration");
    docDocument.appendChild(configElem);

    for(int ii = 0; ii < tabWidget->count(); ++ii)
    {
        QWidget *tab = tabWidget->widget(ii);
        QString tabName = tabWidget->tabText(ii);
        QDomElement tabElem = docDocument.createElement(tabName);
        configElem.appendChild(tabElem);

        if ( tab->layout() != NULL )
        {
            QDomElement filterElem;
            QDomElement keyElem;
            for(int jj = 0; jj < tab->layout()->count(); ++jj)
            {
                QLayoutItem* item = tab->layout()->itemAt(jj);
                QString widgetType = QString(item->widget()->metaObject()->className());
                if(widgetType == "QGroupBox")
                {
                    QString filterName = dynamic_cast<QGroupBox*>(item->widget())->title();
                    filterElem = docDocument.createElement(filterName);
                    tabElem.appendChild(filterElem);
                    continue;
                }
                if(widgetType == "QLabel")
                {
                    keyElem = docDocument.createElement("key");
                    filterElem.appendChild(keyElem);
                    QString labelText = dynamic_cast<QLabel*>(item->widget())->text();
                    keyElem.setAttribute("alias", labelText);
                }
                else
                {
                    if(widgetType == "QLineEdit")
                    {
                        QString lineEdit = dynamic_cast<QLineEdit*>(item->widget())->text();
                        QDomText fieldText = docDocument.createTextNode(lineEdit);
                        keyElem.appendChild(fieldText);
                    }
                    else{
                        /* Then widget is a ComboBox*/
                        QComboBox *comboBox = dynamic_cast<QComboBox*>(item->widget());
                        QStringList list;
                        for(int k = 0; k < comboBox->count(); ++k){
                            list.append(comboBox->itemText(k));
                        }
                        keyElem.setAttribute("list", list.join(','));
                        QDomText currentItem = docDocument.createTextNode(comboBox->currentText());
                        keyElem.appendChild(currentItem);
                    }

                }
            }
        }
    }
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QString(),
                tr("Xml Files (*.xml *.xslt);;Text Files (*.txt);;HTML Files (*.html);;"
                   "All Files (*.*)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QFile::WriteOnly | QFile::Truncate)) {
            QTextStream out(&file);
            docDocument.save(out, 4);
        }
    }
}

void ConfigUI::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), xmlPath,
                            tr("Xml Files (*.xml *.xslt);;HTML files (*.html);;"
                               "User Interface files (*.ui);;All Files (*.*)"));
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly)) {
            QDomDocument document;
            if (document.setContent(&file)) {
                resetUI();
                parseXML(document);
                xmlPath = filePath;
            }
            file.close();
        }
    }

}

void ConfigUI::resetUI()
{
    while(tabWidget->count() > 0)
    {
        QWidget *tab = tabWidget->widget(0);
        if ( tab->layout() != NULL )
        {
            QLayoutItem* item;
            while (tab->layout()->count() > 0)
            {
                item = tab->layout()->takeAt(0);
                delete item->widget();
                delete item;
            }
            delete tab->layout();
        }
        delete tab;
    }
}

void ConfigUI::parseXML(const QDomDocument &document) {

    QDomElement docElem = document.documentElement();
    QString rootTag = docElem.tagName();
    if(rootTag != "configuration"){
        QString error = "Missing root tag <configuration>";
        QMessageBox::critical(this, "TabDialog::parseXML", error, QMessageBox::Ok);
    }

    QDomNodeList siblings = docElem.childNodes();
    for(int i = 0; i < siblings.count(); i++)
    {
        QString tabName = siblings.at(i).toElement().tagName();
        if(tabName == "configSections")
            continue;

        QWidget *tab = new QWidget();
        QFormLayout* layout = new QFormLayout;
        QDomElement el = siblings.at(i).toElement();
        QDomNodeList childList = el.childNodes();

        layout->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
        for(int j = 0; j < childList.count(); j++)
        {
            QDomNode keyNode = childList.at(j).firstChild();
            QString filterName = childList.at(j).toElement().tagName();
            QGroupBox* filterSection = new QGroupBox(filterName);
            layout->addWidget(filterSection);
            /*
             * TODO : Update QGroupBox widget to Flat style
             */
            while(!keyNode.isNull())
            {
                QDomElement keyData = keyNode.toElement();
                QString keyName = keyData.attribute("name");
                QString keyAlias = keyData.attribute("alias");
                QString keyText = keyData.text();

                if(!keyData.attribute("list").isEmpty()){
                    QString keyList = keyData.attribute("list");
                    QStringList keyValues = keyList.split(',');
                    QComboBox *comboBox = new QComboBox();
                    comboBox->addItems(keyValues);
                    if (!keyText.isNull())
                        comboBox->setCurrentText(keyText);
                    layout->addRow(keyAlias, comboBox);
                }
                else{
                    layout->addRow(keyAlias, new QLineEdit(keyText));
                }
                keyNode = keyNode.nextSibling();
            }
        }
        tab->setLayout(layout);
        this->tabWidget->addTab(tab, tabName);
    }
}
