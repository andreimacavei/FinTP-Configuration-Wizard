#include "fintpconfigwizard.h"

#include <QDomElement>
#include <QDomNodeList>
#include <QTextStream>

ConfigUI::ConfigUI(const QString &fileName, QWidget *parent)
    : QMainWindow(parent)
{
    QFileInfo fileInfo(fileName);
    m_xmlPath = fileInfo.absoluteFilePath();
    m_tabWidget = new QTabWidget;

    QFile* file = new QFile(fileName);
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString errorMessage = "Could not open " + fileName;
        QMessageBox::critical(this, "TabDialog::parseXML", errorMessage, QMessageBox::Ok);
        return;
    }

    if (!this->m_Doc.setContent(file)){
        QMessageBox::critical(this, "TabDialog::parseXML",
                              "Unable to set DOM Document", QMessageBox::Ok);
        return;
    }

    parseXML(m_Doc);
    file->close();

    createFrameBox();
    m_addFilter = new QPushButton("Add Filter");
    connect(m_addFilter, SIGNAL(clicked()),this, SLOT(showFrameBox()));
    QToolBar *toolBar = new QToolBar;
    toolBar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    toolBar->addWidget(m_addFilter);

    createMenu();
    this->setMenuBar(m_menuBar);
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(m_tabWidget);
    scrollArea->setWidgetResizable(true);

    this->addToolBar(toolBar);
    setCentralWidget(scrollArea);
    setWindowTitle(tr("FinTP Config GUI"));
}

void ConfigUI::createMenu()
{
    m_menuBar = new QMenuBar;
    m_fileMenu = new QMenu(tr("&File"), this);
    m_fileMenu->addAction(tr("&Open"), this, SLOT(openFile()), QKeySequence::Open);

    QAction *actionSave = new QAction("&Save", this);
    actionSave->setShortcut((QKeySequence::Save));
    QAction *actionSaveAs = new QAction("Save &As", this);
    actionSaveAs->setShortcut(QKeySequence::SaveAs);
    m_fileMenu->addAction(actionSave);
    m_fileMenu->addAction(actionSaveAs);

    m_sigMapper = new QSignalMapper(this);
    connect(actionSave, SIGNAL(triggered()), m_sigMapper, SLOT(map()));
    connect(actionSaveAs, SIGNAL(triggered()), m_sigMapper, SLOT(map()));
    m_sigMapper->setMapping(actionSave, "save");
    m_sigMapper->setMapping(actionSaveAs, "saveAs");
    connect(m_sigMapper, SIGNAL(mapped(QString)), this, SLOT(saveXML(QString)));

    m_fileMenu->addAction(tr("E&xit"), this, SLOT(close()), QKeySequence::Quit);
    m_menuBar->addMenu(m_fileMenu);
}

void ConfigUI::createFrameBox() {

    QGridLayout *layout = new QGridLayout;

    m_listView = new QListView;
    m_listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_frameBox = new QFrame(this, Qt::Tool | Qt::Window);
    m_frameBox->resize(300,200);
    m_frameBox->setFrameStyle ( QFrame::Box | QFrame::Raised);
    m_acceptButton = new QPushButton("Ok");
    m_cancelButton = new QPushButton("Cancel");
    connect(m_acceptButton, SIGNAL(clicked()), this, SLOT(addFilterToGui()));
    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(hideFrameBox()));

    layout->addWidget(m_listView, 0, 0, 1, 0);
    layout->addWidget(m_acceptButton,1,1);
    layout->addWidget(m_cancelButton,1,2);
    m_frameBox->setLayout(layout);
    this->sizeHint();
    m_frameBox->move(this->x() + 150, this->y() + 100);
    m_frameBox->hide();
}

QStandardItemModel* ConfigUI::getFilterList() {

    int index = m_tabWidget->currentIndex();
    QString tabName = m_tabWidget->tabText(index);
    QDomElement tabNode = m_Doc.documentElement().firstChildElement(tabName);
    QDomNodeList filterList = tabNode.childNodes();

    QStandardItemModel* listModel = new QStandardItemModel();
    for (int ii = 0; ii < filterList.count(); ++ii) {
        QDomElement filterNode = filterList.at(ii).toElement();

        if ( filterNode.attribute("visible", "") == "false"){
            QDomNodeList keyList = filterNode.childNodes();

            for (int jj = 0; jj < keyList.count(); ++jj)
            {
                QDomElement keyNode = keyList.at(jj).toElement();
                if ( keyNode.attribute("name", "") == "type"){
                    QString filterText = ": " + keyNode.attribute("alias") + "-"
                            + keyNode.text();
                    QString filterName = filterNode.tagName() + filterText;
                    QStandardItem* listItem = new QStandardItem(filterName);
                    listModel->appendRow(listItem);
                }
            }
        }
    }
    return listModel;
}

void ConfigUI::addFilterToGui() {
    QStringList filterList;
    QModelIndexList listModel = m_listView->selectionModel()->selectedIndexes();
    for(int ii=0; ii < listModel.count(); ++ii) {
        QStringList dataTokens = listModel.at(ii).data(Qt::DisplayRole).toString().split(':');
        filterList.append(dataTokens.at(0));
        printf("\n%i. %s\n", ii+1, filterList.at(ii).toStdString().c_str());
    }

    QWidget *tab = m_tabWidget->currentWidget();
    for(int ii = 0; ii < tab->layout()->count(); ++ii)
    {
        QLayoutItem* item = tab->layout()->itemAt(ii);
        QString widgetType = QString(item->widget()->metaObject()->className());

        if(widgetType == "QGroupBox" && item->widget()->isHidden()){
            QString filterName = static_cast<QGroupBox*>(item->widget())->title();
            if (filterList.contains(filterName, Qt::CaseInsensitive)){
                item->widget()->setVisible(true);
                printf("\n%s\n", filterName.toStdString().c_str());
            }
        }
    }

    m_addFilter->setEnabled(true);
    m_frameBox->hide();
}

void ConfigUI::showFrameBox() {

    m_addFilter->setEnabled(false);
    m_listView->setModel(getFilterList());
    m_frameBox->show();
}

void ConfigUI::hideFrameBox() {

    m_addFilter->setEnabled(true);
    m_frameBox->hide();
}

void ConfigUI::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), m_xmlPath,
                            tr("Xml Files (*.xml *.xslt);;HTML files (*.html);;"
                               "User Interface files (*.ui);;All Files (*.*)"));
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly)) {
            QDomDocument document;
            if (document.setContent(&file)) {
                resetUI();
                parseXML(document);
                m_xmlPath = filePath;
                m_Doc = document;
            }
            file.close();
        }
    }

}

void ConfigUI::resetUI()
{
    while(m_tabWidget->count() > 0)
    {
        QWidget *tab = m_tabWidget->widget(0);
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
            file.setFileName(this->m_xmlPath);
            if (file.open(QFile::WriteOnly | QFile::Truncate)) {
                QTextStream out(&file);
                doc.save(out, 4);
                fileInfo.setFile(file);
            }
        }
    }
    filePath = fileInfo.absoluteFilePath();
    m_xmlPath = filePath;
    file.close();
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    m_Doc.setContent(&file);
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

    QWidget *removedTab = static_cast<QWidget*>(m_removedTabs.at(0));
    m_tabWidget->insertTab(0, removedTab, "configSections");
    for(int ii = 0; ii < m_tabWidget->count(); ++ii)
    {
        QWidget *tab = m_tabWidget->widget(ii);
        QString tabName = m_tabWidget->tabText(ii);
        QDomElement tabElem = domDocument.createElement(tabName);
        configElem.appendChild(tabElem);

        if (ii == 0){
            tabElem.setAttribute("name", removedTab->objectName());
        }
        if ( tab->layout() == NULL ){
            continue;
        }
        QDomElement filterElem;
        QDomElement keyElem;

        for(int jj = 0; jj < tab->layout()->count(); ++jj)
        {
            QLayoutItem* item = tab->layout()->itemAt(jj);
            QString widgetType = QString(item->widget()->metaObject()->className());

            if(widgetType == "QGroupBox"){
                QString filterName = static_cast<QGroupBox*>(item->widget())->title();
                if (tabName == "configSections"){
                    filterElem = domDocument.createElement("sectionGroup");
                    filterElem.setAttribute("name", filterName);
                }
                else{
                    filterElem = domDocument.createElement(filterName);
                    if (item->widget()->objectName() != "")
                        filterElem.setAttribute("visible", item->widget()->objectName());
                }
                tabElem.appendChild(filterElem);
                continue;
            }
            if(widgetType == "QLabel"){
                QString atrName;
                if (tabName == "configSections"){
                    keyElem = domDocument.createElement("section");
                    atrName = "name";
                }
                else{
                    keyElem = domDocument.createElement("key");
                    atrName = "alias";
                    keyElem.setAttribute("name", item->widget()->objectName());
                }
                filterElem.appendChild(keyElem);
                QString labelText = static_cast<QLabel*>(item->widget())->text();
                keyElem.setAttribute(atrName, labelText);
            }
            else{
                if(widgetType == "QLineEdit"){
                    QString lineEdit = static_cast<QLineEdit*>(item->widget())->text();
                    if (lineEdit != ""){
                        QDomText fieldText = domDocument.createTextNode(lineEdit);
                        keyElem.appendChild(fieldText);
                    }
                }
                else{
                    if (widgetType == "QComboBox"){
                        QComboBox *comboBox = dynamic_cast<QComboBox*>(item->widget());
                        if (!comboBox){
                            QString error = "Unknown Widget";
                            QMessageBox::critical(this, "TabDialog::saveXML", error, QMessageBox::Ok);
                        }
                        QStringList list;
                        for(int k = 0; k < comboBox->count(); ++k){
                            list.append(comboBox->itemText(k));
                        }
                        keyElem.setAttribute("list", list.join(','));
                        QDomText currentItem = domDocument.createTextNode(comboBox->currentText());
                        keyElem.appendChild(currentItem);
                    }
                }//end of if(widgetType == "QLineEdit)

            }//end of if(widgetType == "QLabel")
        }//end of for...
    }
    m_tabWidget->removeTab(0);
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
            QString visibleAttr = childList.at(j).toElement().attribute("visible");

            if (filterName == "sectionGroup"){
                filterName = childList.at(j).toElement().attribute("name");
            }
            QGroupBox* filterSectionGroup = new QGroupBox(filterName);
            filterSectionGroup->setObjectName(visibleAttr);
            if (visibleAttr == "false"){
                filterSectionGroup->setHidden(true);
            }
            layout->addWidget(filterSectionGroup);

            /*
             * TODO : Use stretch factor on all groupBoxes
             */

            while(!keyNode.isNull())
            {
                QDomElement keyData = keyNode.toElement();
                QString keyName = keyData.attribute("name");
                QString keyAlias = keyData.attribute("alias");
                QString keyText = keyData.text();
                QLabel *keyLabel;

                if(!keyData.attribute("list").isEmpty()){
                    QString keyList = keyData.attribute("list");
                    QStringList keyValues = keyList.split(',');
                    QComboBox *comboBox = new QComboBox();
                    comboBox->addItems(keyValues);
                    if (!keyText.isNull())
                        comboBox->setCurrentText(keyText);
                    keyLabel = new QLabel(keyAlias);
                    keyLabel->setObjectName(keyName);
                    layout->addRow(keyLabel, comboBox);
                }
                else{
                    if (keyData.tagName() == "section")
                    {
                        keyAlias = keyName;
                    }
                    keyLabel = new QLabel(keyAlias);
                    keyLabel->setObjectName(keyName);
                    layout->addRow(keyLabel, new QLineEdit(keyText));
                }
                keyNode = keyNode.nextSibling();
            }
        }

        tab->setLayout(layout);
        this->m_tabWidget->addTab(tab, tabName);
        if(tabName == "configSections"){
            tab->setObjectName(siblings.at(i).toElement().attribute("name"));
            m_removedTabs.append(tab);
            this->m_tabWidget->removeTab(pos);
        }
        pos++;
    }
}
