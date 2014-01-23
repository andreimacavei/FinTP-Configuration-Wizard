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
 * methods to parse a Xml file and generate the GUI.
 *
 */
class ConfigUI: public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief ConfigUI  Constructs the main window
     *
     * @param fileName  the file used to generate the UI
     * @param parent  the parent widget of all widgets used
     */
    explicit ConfigUI(const QString &fileName, QWidget *parent = 0);

private slots:
    /**
     * @brief writeFileStream  Writes the QDomDocument to a file on disk
     *
     * @param doc  the QDomDocument to be written on file.
     * @param saveType  a string used to decide if the Dom document should be
     * saved on a new file or override the file that has generated current GUI.
     */
    void writeFileStream(QDomDocument doc, QString saveType);

    /**
     * @brief parseXML  Parses the input xml file using a QDomDocument
     * then generates the GUI.
     * @param doc  the QDomDocument class used to parse a xml file.
     */
    void parseXML(const QDomDocument &doc);

    /**
     * @brief resetUI  De-allocates memory for all widgets objects from  the
     * main window.
     */
    void resetUI();

    /**
     * @brief showFrameBox  Displays the frame box from which user can
     * add new filters.
     */
    void showFrameBox(QString buttonPressed);

    /**
     * @brief hideFrameBox  Hides the frame box from the screen.
     */
    void hideFrameBox();

    /**
     * @brief processFilterToGui
     * @param buttonPressed
     */
    void processFilterToGui();

    /**
     * @brief addFilterToGui  Displays the selected filters on the current tab
     * of the GUI, as a GroupBox widget.
     */
    void addFilterToGui();

    /**
     * @brief removeFilter  Removes the selected filters from the current tab.
     */
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
     *
     * @param saveType  a string used to decide if the Dom document should be
     * saved on a new file or override the file that has generated current GUI.
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

    /**
     * @brief updateFilterToXml  Updates the visible attribute of a
     * filter in the xml file (visible = true or false).
     *
     * @param tabName  The tab (or main filter) to which this filter belongs
     * @param filterName  The filter name to have its visible attribute updated
     * @param filterAttr  true or false depending if the filter should be
     * visible or not in the GUI.
     */
    void updateFilterToXml(const QString tabName, const QString filterName,
                           QString filterAttr);

    /**
     * @brief getFilterFromXml  Retrieves the missing filters from the
     * opened xml file, and returns a list with those filters corresponding to
     * the currently selected tab.
     *
     * @return  a list with the missing filters as a QStandardItemModel object
     */
    QStandardItemModel* getFilterFromXml(QString buttonPressed);

    QDomElement addElement( QDomDocument &doc, QDomNode &node, const QString &tag,
                            const QString &value );
    QTabWidget *m_tabWidget;
    QList<QWidget*> m_removedTabs;
    QDialogButtonBox *m_buttonBox;
    QMenuBar *m_menuBar;
    QMenu *m_fileMenu;
    QDomDocument m_Doc;
    QSignalMapper *m_sigMapper1;
    QSignalMapper *m_sigMapper2;
    QList<QPushButton*> m_buttonList;
    QPushButton *m_addFilter;
    QPushButton *m_delFilter;
    QPushButton *m_acceptButton;
    QPushButton *m_cancelButton;
    QFrame *m_frameBox;
    QListView *m_listView;
    QString m_xmlPath;
    QString m_buttonPressed;
};


#endif // FINTPCONFIGWIZARD_H
