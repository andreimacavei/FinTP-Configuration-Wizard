#include "fintpconfigwizard.h"
#include "ui_fintpconfigwizard.h"


TabDialog::TabDialog(const QString &fileName, QWidget *parent)
    : QDialog(parent)
{
    tabWidget = new QTabWidget;

    /*
     * TO DO: Populate tabWidget with tabs retrieved from parsed XML
     */

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
    /* We'll parse the input xml file */
    QFile* file = new QFile(fileName);
    /* If we can't open it, let's show an error message. */
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString error_message = "Couldn't open " + fileName;
        QMessageBox::critical(this,
                              "TabDialog::parseXML",
                              error_message,
                              QMessageBox::Ok);
        return;
    }
    /* QXmlStreamReader takes any QIODevice. */
    QXmlStreamReader xml(file);
    QList< QMap<QString,QString> > sections;
    /* We'll parse the XML until we reach end of it.*/
    while(!xml.atEnd() &&
            !xml.hasError()) {
        /* Read next element.*/
        QXmlStreamReader::TokenType token = xml.readNext();
        /* If token is just StartDocument, we'll go to next.*/
        if(token == QXmlStreamReader::StartDocument) {
            continue;
        }
        /* If token is StartElement, we'll see if we can read it.*/
        if(token == QXmlStreamReader::StartElement) {
            /* If it's named configuration, we'll go to the next.*/
            if(xml.name() == "configuration") {
                continue;
            }
            /* If it's named sectionGroup, we'll dig the information from there.*/
            if(xml.name() == "sectionGroup") {
                /*
                 * TO DO: get a copy of current element from xml object
                 */
                sections.append(this->parseSectionGroup(xml));
            }
        }
    }
    /* Error handling. */
    if(xml.hasError()) {
        QMessageBox::critical(this,
                              "TabDialog::parseXML",
                              xml.errorString(),
                              QMessageBox::Ok);
    }
    /* Removes any device() or data from the reader
     * and resets its internal state to the initial state. */
    xml.clear();
    this->addSectionsToUI(sections);
}

QMap<QString, QString> TabDialog::parseSectionGroup(QXmlStreamReader& xml) {
    QMap<QString, QString> sectionGroup;
    /* Let's check that we're really getting a sectionGroup. */
    if(xml.tokenType() != QXmlStreamReader::StartElement &&
            xml.name() == "sectionGroup") {
        return sectionGroup;
    }
    /* Let's get the attributes for sectionGroup */
    QXmlStreamAttributes attributes = xml.attributes();
    /* Let's check that sectionGroup has name attribute. */
    if(attributes.hasAttribute("name")) {
        /* We'll add it to the map. */
        sectionGroup["name"] = attributes.value("name").toString();
    }
    /* Next element... */
    xml.readNext();
    /*
     * We're going to loop over the things because the order might change.
     * We'll continue the loop until we hit an EndElement named sectionGroup.
     */
    while(!(xml.tokenType() == QXmlStreamReader::EndElement &&
            xml.name() == "sectionGroup")) {
        if(xml.tokenType() == QXmlStreamReader::StartElement) {
            /* We add all section filters to Map. */
            if(xml.name() == "section") {
                this->addElementDataToMap(xml, sectionGroup);
            }
        }
        /* ...and next... */
        xml.readNext();
    }
    return sectionGroup;
}

void TabDialog::addElementDataToMap(QXmlStreamReader& xml,
                                      QMap<QString, QString>& map) const {
    /* We need a start element, like <foo> */
    if(xml.tokenType() != QXmlStreamReader::StartElement) {
        return;
    }
    /* Let's read the filter */

    QXmlStreamAttributes attributes = xml.attributes();
    if(!attributes.hasAttribute("name")){
        return;
    }
    /* ...go to the next. */
    xml.readNext();

    /* Now we can add it to the map.*/
    map.insert(attributes.value("name").toString(), xml.text().toString());
}

void TabDialog::addSectionsToUI(QList< QMap<QString,QString> >& sections) {

    while(!sections.isEmpty()) {
        QWidget *tab = new QWidget();

        QFormLayout* layout = new QFormLayout;
        QMap<QString,QString> section = sections.takeFirst();

        //QGroupBox* sectionBox = new QGroupBox(tr(sections["name"]));
        QMap<QString, QString>::const_iterator iter = section.constBegin();
        while ( iter != section.constEnd()){
            layout->addRow(iter.key(), new QLineEdit(iter.value()));
            ++iter;
        }
        tab->setLayout(layout);
        this->tabWidget->addTab(tab, section.value("name", "Unknown"));
    }
}
