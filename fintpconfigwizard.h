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
    void parseXML(const QString &fileName);

public slots:
    void saveFile();
    void updateFile();

private:
    QDomElement addElement( QDomDocument &doc, QDomNode &node, const QString &tag,
                            const QString &value );
    QTabWidget *tabWidget;
    QDialogButtonBox *buttonBox;
    QMenu *fileMenu;
    QDomDocument doc;
};


#endif // FINTPCONFIGWIZARD_H
