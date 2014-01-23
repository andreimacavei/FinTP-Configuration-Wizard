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

/**
 * @brief The ConfigUI class
 * The main class for our configuration GUI that holds all the necessary
 * methods to parse XML file and generate the GUI
 *
 */
class ConfigUI: public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief ConfigUI  Constructs the main window
     * @param fileName  the file used to generate the UI
     * @param parent  the parent widget of all widgets used
     */
    explicit ConfigUI(const QString &fileName, QWidget *parent = 0);

private slots:
    /**
     * @brief writeFileStream  Writes the QDomDocument to a file on disk
     * @param doc the QDomDocument to be written on file.
     * @param saveType used to decide between save to a new file or override
     * the current file.
     */
    void writeFileStream(QDomDocument doc, QString saveType);

    /**
     * @brief parseXML  Parses the input xml file using a QDomDocument
     * object, then generates the GUI.
     * @param doc  the QDomDocument object used to parse a xml file.
     */
    void parseXML(const QDomDocument &doc);

    /**
     * @brief resetUI  De-allocates all widgets objects from  the
     * main window.
     */
    void resetUI();

    /**
     * @brief showFrameBox  Displays the frame box from where user can
     * add new filters.
     */
    void showFrameBox();

    /**
     * @brief hideFrameBox  Hides the frame box from the screen.
     */
    void hideFrameBox();

    /**
     * @brief addFilterToGui  Displays the selected filters on the current tab
     * of the GUI, as a GroupBox Widget
     */
    void addFilterToGui();
    void removeFilter();

public slots:
    /**
     * @brief openFile  Shows a dialog box for user to choose which
     * file to be opened, then resets and generates the GUI from this file.
     */
    void openFile();

    /**
     * @brief saveXML  Saves the GUI to a Dom parser using
     * a QDomDocument object
     * @param saveType
     */
    void saveXML(QString saveType);

private:
    /**
     * @brief createMenu  Creates a menu bar for the main window using
     * a QMenuBar widget.
     */
    void createMenu();

    /**
     * @brief createFrameBox  Creates a FrameBox from which user can
     * select and add missing filters to the current tab.
     */
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
    QPushButton *m_delFilter;
    QPushButton *m_acceptButton;
    QPushButton *m_cancelButton;
    QFrame *m_frameBox;
    QListView *m_listView;
};


#endif // FINTPCONFIGWIZARD_H
