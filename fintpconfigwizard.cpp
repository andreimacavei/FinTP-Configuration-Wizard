#include "fintpconfigwizard.h"

#include <QDomElement>
#include <QDomNodeList>
#include <QTextStream>

/**
 * @brief ConfigUI::ConfigUI  - Constructs the main window using a
 *  QMainWindow object.
 *
 * @param fileName  the file used to generate the UI
 * @param parent  the parent widget of all widgets used
 */
ConfigUI::ConfigUI(const QString &fileName, QWidget *parent)
    : QMainWindow(parent)
{

    QFileInfo fileInfo(fileName);
    m_xmlPath = fileInfo.absoluteFilePath();

    // Open the XML file to generate configuration GUI
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

    // We're using a Tab widget to hold each main filter on separate tab.
    m_tabWidget = new QTabWidget;
    parseXML(m_Doc);
    file->close();

    // Adding additional widgets to our GUI: frame box, toolbar with buttons
    createFrameBox();

    m_addFilter = new QPushButton("Add Filter");    // button for add filter
    connect(m_addFilter, SIGNAL(clicked()),this, SLOT(showFrameBox()));
    m_delFilter = new QPushButton("Remove Filter"); // button for delete filter
    connect(m_addFilter, SIGNAL(clicked()),this, SLOT(removeFilter()));
    m_delFilter->setEnabled(false);

    QToolBar *toolBar = new QToolBar;
    toolBar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    toolBar->addWidget(m_addFilter);
    toolBar->addWidget(m_delFilter);

    // Adding a menu and a scroll area for main window of GUI
    createMenu();
    this->setMenuBar(m_menuBar);

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(m_tabWidget);
    scrollArea->setWidgetResizable(true);

    this->addToolBar(toolBar);
    setCentralWidget(scrollArea);
    setWindowTitle(tr("FinTP Config GUI"));
}

/**
 * @brief ConfigUI::createMenu  - Creates a menu bar for the main window using
 * a QMenuBar widget.
 */
void ConfigUI::createMenu()
{
    // Creating the menu bar with its menus
    m_menuBar = new QMenuBar;
    m_fileMenu = new QMenu(tr("&File"), this);

    // Adding FILE menu's entries with their associated actions
    // File -> Open
    m_fileMenu->addAction(tr("&Open"), this, SLOT(openFile()), QKeySequence::Open);
    // File -> Save
    QAction *actionSave = new QAction("&Save", this);
    actionSave->setShortcut((QKeySequence::Save));
    // File -> SaveAs
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

    // File -> Exit
    m_fileMenu->addAction(tr("E&xit"), this, SLOT(close()), QKeySequence::Quit);

    m_menuBar->addMenu(m_fileMenu);
}

/**
 * @brief ConfigUI::createFrameBox  - Creates a FrameBox from which user can
 *  select and add missing filters to the current tab.
 */
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
    m_frameBox->move(this->x() + 500, this->y() + 300);
    m_frameBox->hide();
}

/**
 * @brief ConfigUI::getFilterFromXml  - Retrieves the missing filters from the
 * opened xml file, and returns a list with those filters corresponding to the
 * currently selected tab.
 *
 * @return  a list with the missing filters as a QStandardItemModel object
 */
QStandardItemModel* ConfigUI::getFilterFromXml() {

    int tabIndex = m_tabWidget->currentIndex();
    QString tabName = m_tabWidget->tabText(tabIndex);
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

/**
 * @brief ConfigUI::updateFilterToXml  Updates the visible attribute of a
 * filter in the xml file (visible = true or false).
 *
 * @param tabName  The tab (or main filter) to which this filter belongs
 * @param filterName  The filter name to have its visible attribute updated
 * @param filterAttr  true or false depending if the filter should be visible
 * or not in the GUI.
 */
void ConfigUI::updateFilterToXml(QString tabName, QString filterName,
                                 QString filterAttr) {
    m_Doc.documentElement().firstChildElement(tabName).
            firstChildElement(filterName).setAttribute("visible", filterAttr);
}

/**
 * @brief ConfigUI::addFilterToGui  Displays the selected filters on the
 * current tab of the GUI, as a GroupBox Widget.
 */
void ConfigUI::addFilterToGui() {
    QStringList filterList;
    QModelIndexList listModel = m_listView->selectionModel()->selectedIndexes();
    for(int ii=0; ii < listModel.count(); ++ii) {
        QStringList dataTokens = listModel.at(ii).data(Qt::DisplayRole).
                toString().split(':');
        filterList.append(dataTokens.at(0));
    }

    QWidget *tab = m_tabWidget->currentWidget();
    int tabIndex = m_tabWidget->currentIndex();
    QString tabName = m_tabWidget->tabText(tabIndex);

    for(int ii = 0; ii < tab->layout()->count(); ++ii)
    {
        QLayoutItem* item = tab->layout()->itemAt(ii);
        QString widgetType = QString(item->widget()->metaObject()->className());

        if(widgetType == "QGroupBox" && item->widget()->isHidden()){
            QString filterName = static_cast<QGroupBox*>(item->widget())->title();
            if (filterList.contains(filterName, Qt::CaseInsensitive)){
                item->widget()->setVisible(true);
                updateFilterToXml(tabName, filterName, "true");
            }
        }
    }

    m_addFilter->setEnabled(true);
    m_delFilter->setEnabled(true);
    m_frameBox->hide();
}

void ConfigUI::removeFilter() {

}

/**
 * @brief ConfigUI::showFrameBox  Displays the frame box from which user can
 * add new filters.
 */
void ConfigUI::showFrameBox() {

    m_addFilter->setEnabled(false);
    m_delFilter->setEnabled(false);
    m_listView->setModel(getFilterFromXml());
    m_frameBox->show();
}

/**
 * @brief ConfigUI::hideFrameBox  Hides the frame box from the screen.
 */
void ConfigUI::hideFrameBox() {

    m_addFilter->setEnabled(true);
    m_delFilter->setEnabled(true);
    m_frameBox->hide();
}

/**
 * @brief ConfigUI::openFile  Shows a dialog box for user to choose which
 * file to be opened, then resets and generates the GUI from this file.
 */
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

/**
 * @brief ConfigUI::resetUI  De-allocates memory for all widgets objects from
 * the main window.
 */
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

/**
 * @brief ConfigUI::writeFileStream  Writes the QDomDocument to a file on disk
 *
 * @param doc  the QDomDocument to be written on file.
 * @param saveType  a string used to decide if the Dom document should be
 * saved on a new file or override the file that has generated current GUI.
 */
void ConfigUI::writeFileStream(QDomDocument doc, QString saveType)
{
    QFileInfo fileInfo;
    QString filePath;
    QFile file;

    // Depending on the saveType param we save the Dom document to a new file
    // or to the file that has generated current GUI.
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

/**
 * @brief ConfigUI::saveXML  Save the GUI to a Dom document using
 * QDomDocument class
 * @param saveType  a string used to decide if the Dom document should be
 * saved on a new file or override the file that has generated current GUI.
 */
void ConfigUI::saveXML(QString saveType)
{
    // Creates a QDomDocument to save the current GUI
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
        // Each Tab is mapped to a child node (main Filter) of the root
        // tag in DOM document
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

        // For each widget in the Tab we create child nodes in the
        // DOM document tree
        for(int jj = 0; jj < tab->layout()->count(); ++jj)
        {
            QLayoutItem* groupBox = tab->layout()->itemAt(jj);
            QString filterName = static_cast<QGroupBox*>(groupBox->widget())->title();
            if (tabName == "configSections"){
                filterElem = domDocument.createElement("sectionGroup");
                filterElem.setAttribute("name", filterName);
            }
            else{
                filterElem = domDocument.createElement(filterName);
                if (groupBox->widget()->objectName() != "") {
                    QString filterAttr = groupBox->widget()->objectName();
                    if((filterAttr == "false") &&
                            !groupBox->widget()->isHidden())
                        filterAttr = "true";
                    filterElem.setAttribute("visible", filterAttr);
                }
            }
            tabElem.appendChild(filterElem);

            for(int k = 0; k < groupBox->widget()->layout()->count(); ++k)
            {
                QLayoutItem* item = groupBox->widget()->layout()->itemAt(k);
                QString widgetType = QString(item->widget()->metaObject()->className());
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
                            for(int m = 0; m < comboBox->count(); ++m){
                                list.append(comboBox->itemText(m));
                            }
                            keyElem.setAttribute("list", list.join(','));
                            QDomText currentItem = domDocument.createTextNode(comboBox->currentText());
                            keyElem.appendChild(currentItem);
                        }
                    }//end of if(widgetType == "QLineEdit)
                }//end of if(widgetType == "QLabel")
            }//end of for(int k = 0;...)
        }//end of for(int jj = 0;...)
    }
    m_tabWidget->removeTab(0);
    writeFileStream(domDocument, saveType);
}

/**
 * @brief ConfigUI::parseXML  Parses the input xml file using a QDomDocument
 * then generates the GUI.
 * @param document  the QDomDocument class used to parse a xml file
 */
void ConfigUI::parseXML(const QDomDocument &document) {

    // Get the root of this XML
    QDomElement docElem = document.documentElement();
    QString rootTag = docElem.tagName();
    if(rootTag != "configuration"){
        QString errorMessage = "Missing root tag <configuration>";
        QMessageBox::critical(this, "TabDialog::parseXML", errorMessage, QMessageBox::Ok);
    }

    int pos = 0;
    // Get all direct child nodes (main Filters) for root tag.
    QDomNodeList siblings = docElem.childNodes();
    for(int i = 0; i < siblings.count(); i++)
    {
        // For each main Filters we create a separate Tab widget
        QString tabName = siblings.at(i).toElement().tagName();
        QWidget *tab = new QWidget();

        QFormLayout* layout = new QFormLayout;
        QDomElement el = siblings.at(i).toElement();
        QDomNodeList childList = el.childNodes();
        layout->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);

        // Get all child nodes (child Filters) for this main Filter
        for(int j = 0; j < childList.count(); j++)
        {
            // For each child Filter we create a GroupBox widget on current tab

            QString filterName = childList.at(j).toElement().tagName();
            QString visibleAttr = childList.at(j).toElement().attribute("visible", "");

            if (filterName == "sectionGroup"){
                filterName = childList.at(j).toElement().attribute("name");
            }

            QGroupBox* filterSectionGroup = new QGroupBox(filterName);
            QFormLayout *groupLayout = new QFormLayout;

            filterSectionGroup->setObjectName(visibleAttr);
            if (visibleAttr == "false"){
                filterSectionGroup->setHidden(true);
            }
            layout->addWidget(filterSectionGroup);

            /*
             * TODO : Modify implicit stretch factor on all groupBoxes
             */

            // Get all keys attributes for this filter.
            QDomNode keyNode = childList.at(j).firstChild();
            while(!keyNode.isNull())
            {
                // For each key we create a different GUI widget depending
                // on it's functionality: combo box, line edit or label
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
                    groupLayout->addRow(keyLabel, comboBox);
                }
                else{
                    if (keyData.tagName() == "section")
                    {
                        keyAlias = keyName;
                    }
                    keyLabel = new QLabel(keyAlias);
                    keyLabel->setObjectName(keyName);
                    groupLayout->addRow(keyLabel, new QLineEdit(keyText));
                }
                keyNode = keyNode.nextSibling();
            }
            filterSectionGroup->setLayout(groupLayout);
        }

        // Finnaly we add our Tab object to the main Tab widget
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
