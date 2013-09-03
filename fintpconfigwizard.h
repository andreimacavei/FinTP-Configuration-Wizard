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


class ConfigUI: public QMainWindow
{
    Q_OBJECT

public:
    explicit ConfigUI(const QString &fileName, QWidget *parent = 0);

private slots:
    void parseXML(const QDomDocument &doc);
    void resetUI();

public slots:
    void openFile();
    void saveFile();
    void saveFileAs(QString);

private:
    QDomElement addElement( QDomDocument &doc, QDomNode &node, const QString &tag,
                            const QString &value );
    void writeFileStream(QDomDocument doc, QString fileName);
    QTabWidget *tabWidget;
    QList<QWidget*> removedTabs;
    QDialogButtonBox *buttonBox;
    QMenu *fileMenu;
    QString xmlPath;
    QDomDocument doc;
    QSignalMapper *sigMapper;
};


#endif // FINTPCONFIGWIZARD_H
