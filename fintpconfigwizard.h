#ifndef FINTPCONFIGWIZARD_H
#define FINTPCONFIGWIZARD_H

#include <QDialog>
#include <QXmlStreamReader>
#include <QtWidgets>

#include <QtCore/QFile>
#include <QtCore/QIODevice>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>

#include <QDomDocument>

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QTabWidget;
QT_END_NAMESPACE


class ConfigUI: public QDialog
{
    Q_OBJECT

public:
    explicit ConfigUI(const QString &fileName, QWidget *parent = 0);

private slots:
    void writeFileStream(QDomDocument doc, QString saveType);
    void parseXML(const QDomDocument &doc);
    void resetUI();

public slots:
    void openFile();
    void saveXML(QString saveType);

private:
    void createMenu();
    QDomElement addElement( QDomDocument &doc, QDomNode &node, const QString &tag,
                            const QString &value );

    QMenuBar *m_menuBar;
    QMenu *m_fileMenu;

    QTabWidget *m_tabWidget;
    QList<QWidget*> m_removedTabs;
    QDialogButtonBox *m_buttonBox;
    QString m_xmlPath;
    QDomDocument m_Doc;
    QSignalMapper *m_sigMapper;
};


#endif // FINTPCONFIGWIZARD_H
