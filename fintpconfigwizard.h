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
    void writeFileStream(QDomDocument doc, QString saveType);
    void parseXML(const QDomDocument &doc);
    void resetUI();
    void showFrameBox();
    void hideFrameBox();
    void addFilterToGui();

public slots:
    void openFile();
    void saveXML(QString saveType);

private:
    void createMenu();
    void createFrameBox();
    void updateFilterToXml(const QString tabName, const QString filterName,
                           QString filterAttr);
    QStandardItemModel* getFilterFromXml();
    QDomElement addElement( QDomDocument &doc, QDomNode &node, const QString &tag,
                            const QString &value );
    QTabWidget *m_tabWidget;
    QList<QWidget*> m_removedTabs;
    QDialogButtonBox *m_buttonBox;
    QMenuBar *m_menuBar;
    QMenu *m_fileMenu;
    QString m_xmlPath;
    QDomDocument m_Doc;
    QSignalMapper *m_sigMapper;
    QList<QPushButton*> m_buttonList;
    QPushButton *m_addFilter;
    QPushButton *m_acceptButton;
    QPushButton *m_cancelButton;
    QFrame *m_frameBox;
    QListView *m_listView;
};


#endif // FINTPCONFIGWIZARD_H
