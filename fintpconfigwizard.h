#ifndef FINTPCONFIGWIZARD_H
#define FINTPCONFIGWIZARD_H

#include <QDialog>
#include <QXmlStreamReader>

/*
#include <QMainWindow>

namespace Ui {
class FinTPConfigWizard;
}

class FinTPConfigWizard : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit FinTPConfigWizard(QWidget *parent = 0);
    ~FinTPConfigWizard();
    
private:
    Ui::FinTPConfigWizard *ui;
};
*/

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QTabWidget;
QT_END_NAMESPACE

/*
class GeneralTab : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralTab(QWidget *parent = 0);
};
*/

class TabDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TabDialog(QWidget *parent = 0);

private slots:
    void parseXML();

private:
    QTabWidget *tabWidget;
    QDialogButtonBox *buttonBox;

    QMap<QString, QString> parseSectionGroup(QXmlStreamReader& xml);
    void addElementDataToMap(QXmlStreamReader& xml,
                             QMap<QString, QString>& map) const;

    void addSectionsToUI(QList< QMap<QString,QString> >& persons);
};


#endif // FINTPCONFIGWIZARD_H
