#include "fintpconfigwizard.h"

#include <QDomElement>
#include <QDomNodeList>
#include <QTextStream>

ConfigUI::ConfigUI(const QString &fileName, QWidget *parent)
    : QMainWindow(parent)
{
    QFileInfo fileInfo(fileName);
    xmlPath = fileInfo.absoluteFilePath();
    printf("\nFirst xml path: %s\n", xmlPath.toStdString().c_str());
    tabWidget = new QTabWidget;
    QFile* file = new QFile(fileName);

    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString errorMessage = "Couldn't open " + fileName;
        QMessageBox::critical(this, "TabDialog::parseXML", errorMessage, QMessageBox::Ok);
        return;
    }
    if (!this->doc.setContent(file)){
        QMessageBox::critical(this, "TabDialog::parseXML", "Unable to set DOM parser", QMessageBox::Ok);
        return;
    }
    parseXML(doc);
    file->close();

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Open"), this, SLOT(openFile()), QKeySequence::Open);

    QAction *actionSave = new QAction("&Save", this);
    actionSave->setShortcut((QKeySequence::Save));
    QAction *actionSaveAs = new QAction("Save &As", this);
    actionSaveAs->setShortcut(QKeySequence::SaveAs);
    fileMenu->addAction(actionSave);
    fileMenu->addAction(actionSaveAs);

    sigMapper = new QSignalMapper(this);
    connect(actionSave, SIGNAL(triggered()), sigMapper, SLOT(map()));
    connect(actionSaveAs, SIGNAL(triggered()), sigMapper, SLOT(map()));
    sigMapper->setMapping(actionSave, "save");
    sigMapper->setMapping(actionSaveAs, "saveAs");
    connect(sigMapper, SIGNAL(mapped(QString)), this, SLOT(saveXML(QString)));

//    connect(sigMapper, SIGNAL(mapped(QString)), this, SLOT(saveFile(QString)));
//    connect(sigMapper, SIGNAL(mapped(QString)), this, SLOT(saveFileAs(QString)));

//    fileMenu->addAction(tr("&Save"), this, SLOT(saveFile()), QKeySequence::Save);
//    fileMenu->addAction(tr("Save &As"), this, SLOT(saveFileAs()), QKeySequence::SaveAs);

    fileMenu->addAction(tr("E&xit"), this, SLOT(close()), QKeySequence::Quit);

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(tabWidget);
    scrollArea->setWidgetResizable(true);

    setCentralWidget(scrollArea);
    setWindowTitle(tr("FinTP Config GUI"));
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
                printf("\nNew XML path: %s\n", xmlPath.toStdString().c_str());
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

void ConfigUI::writeFileStream(QDomDocument doc, QString saveType)
{
    QFileInfo fileInfo;
    QString filePath;
    QFile file;

    if(saveType == "saveAs"){
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QString(),
                    tr("Xml Files (*.xml *.xslt);;Text Files (*.txt);;HTML Files (*.html);;"
                           "All Files (*.*)"));
        if (!fileName.isEmpty()) {
            file.setFileName(fileName);
            if (file.open(QFile::WriteOnly | QFile::Truncate)) {
                QTextStream out(&file);
                doc.save(out, 4);
                fileInfo.setFile(file);
            }
        }
    }
    else{
        if(saveType == "save"){
            file.setFileName(this->xmlPath);
            if (file.open(QFile::WriteOnly | QFile::Truncate)) {
                QTextStream out(&file);
                doc.save(out, 4);
                fileInfo.setFile(file);
            }
        }
    }
    filePath = fileInfo.absoluteFilePath();
    xmlPath = filePath;
    file.close();
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

void ConfigUI::saveXML(QString saveType)
{
    QDomDocument domDocument;
    QDomProcessingInstruction instr = domDocument.createProcessingInstruction(
                        "xml", "version='1.0' encoding='UTF-8'");
    domDocument.appendChild(instr);
    QDomElement configElem = domDocument.createElement("configuration");
    domDocument.appendChild(configElem);

    QWidget *removedTab = static_cast<QWidget*>(removedTabs.at(0));
    tabWidget->insertTab(0, removedTab, "configSections");
    for(int ii = 0; ii < tabWidget->count(); ++ii)
    {
        QWidget *tab = tabWidget->widget(ii);
        QString tabName = tabWidget->tabText(ii);
        QDomElement tabElem = domDocument.createElement(tabName);
        configElem.appendChild(tabElem);

        if (ii == 0){
            tabElem.setAttribute("name", removedTab->objectName());
        }
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
                    QString filterName = static_cast<QGroupBox*>(item->widget())->title();
                    if (ii == 0){
                        filterElem = domDocument.createElement("sectionGroup");
                        filterElem.setAttribute("name", filterName);
                    }
                    else{
                        filterElem = domDocument.createElement(filterName);
                    }
                    filterElem = domDocument.createElement(filterName);
                    tabElem.appendChild(filterElem);
                    continue;
                }
                if(widgetType == "QLabel")
                {
                    QString atrName;
                    if (tabName == "configSection"){
                        keyElem = domDocument.createElement("section");
                        atrName = "name";
                    }
                    else{
                        keyElem = domDocument.createElement("key");
                        atrName = "alias";
                    }
                    filterElem.appendChild(keyElem);
                    QString labelText = static_cast<QLabel*>(item->widget())->text();
                    keyElem.setAttribute(atrName, labelText);
                    keyElem.setAttribute("alias", labelText);
                }
                else
                {
                    if(widgetType == "QLineEdit")
                    {
                        QString lineEdit = static_cast<QLineEdit*>(item->widget())->text();
                        QDomText fieldText = domDocument.createTextNode(lineEdit);
                        keyElem.appendChild(fieldText);
                    }
                    else{
                        /* Then widget is a ComboBox*/
                        QComboBox *comboBox = dynamic_cast<QComboBox*>(item->widget());
                        if (!comboBox){
                            QString error = "Unknown Widget";
                            QMessageBox::critical(this, "TabDialog::saveFileAs", error, QMessageBox::Ok);
                        }
                        QStringList list;
                        for(int k = 0; k < comboBox->count(); ++k){
                            list.append(comboBox->itemText(k));
                        }
                        keyElem.setAttribute("list", list.join(','));
                        QDomText currentItem = domDocument.createTextNode(comboBox->currentText());
                        keyElem.appendChild(currentItem);
                    }

                }
            }
        }
    }
    tabWidget->removeTab(0);
    removedTabs.removeFirst();
    writeFileStream(domDocument, saveType);
}

void ConfigUI::parseXML(const QDomDocument &document) {

    QDomElement docElem = document.documentElement();
    QString rootTag = docElem.tagName();
    if(rootTag != "configuration"){
        QString errorMessage = "Missing root tag <configuration>";
        QMessageBox::critical(this, "TabDialog::parseXML", errorMessage, QMessageBox::Ok);
    }

    int pos = 0;
    QDomNodeList siblings = docElem.childNodes();
    for(int i = 0; i < siblings.count(); i++)
    {
        QString tabName = siblings.at(i).toElement().tagName();
        QWidget *tab = new QWidget();
        QFormLayout* layout = new QFormLayout;
        QDomElement el = siblings.at(i).toElement();
        QDomNodeList childList = el.childNodes();
        layout->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);

        for(int j = 0; j < childList.count(); j++)
        {
            QDomNode keyNode = childList.at(j).firstChild();
            QString filterName = childList.at(j).toElement().tagName();
            if (filterName == "sectionGroup"){
                filterName = childList.at(j).toElement().attribute("name");
            }
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
                    if (keyData.tagName() == "section")
                    {
                        keyAlias = keyName;
                    }
                    layout->addRow(keyAlias, new QLineEdit(keyText));
                }
                keyNode = keyNode.nextSibling();
            }
        }
        tab->setLayout(layout);
        this->tabWidget->addTab(tab, tabName);
        if(tabName == "configSections"){
            tab->setObjectName(siblings.at(i).toElement().attribute("name"));
            removedTabs.append(tab);
            this->tabWidget->removeTab(pos);
        }
        pos++;
    }
}
