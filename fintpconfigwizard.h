#ifndef FINTPCONFIGWIZARD_H
#define FINTPCONFIGWIZARD_H

#include <QDialog>
#include <QXmlStreamReader>
#include <QtWidgets>

#include <QtCore/QPointer>
#include <QtCore/QFile>
#include <QtCore/QIODevice>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>

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


class TabDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TabDialog(const QString &fileName, QWidget *parent = 0);

private slots:
    void parseXML(const QString &fileName);

private:
    QTabWidget *tabWidget;
    QDialogButtonBox *buttonBox;

    QMap<QString, QString> parseSectionGroup(QXmlStreamReader& xml);
    void addElementDataToMap(QXmlStreamReader& xml,
                             QMap<QString, QString>& map) const;

    void addSectionsToUI(QList< QMap<QString,QString> >& sections);
};


#endif // FINTPCONFIGWIZARD_H
