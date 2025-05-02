#ifndef RIBBONBAR_H
#define RIBBONBAR_H

#include <QTabWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QAction>
#include <QDialog>
#include <QDateEdit>
#include <QCheckBox>
#include <QMenu>
#include <QToolButton>
#include <QStringList>
#include "fileviewmodel.h"

class AdvancedSearchDialog;

class RibbonBar : public QWidget {
    Q_OBJECT
public:
    explicit RibbonBar(QWidget *parent = nullptr);
    QLineEdit* getAddressBar() const { return addressBar; }
    QLineEdit* getSearchBar() const { return searchBar; }
    void updateRecentFolders(const QString& path);

private:
    QWidget *createFileTab();
    QWidget *createHomeTab();
    QWidget *createShareTab();
    QWidget *createViewTab();
    QLineEdit* addressBar;
    QLineEdit* searchBar;
    QAction* recentFoldersAction;
    QMenu* recentFoldersMenu;
    QStringList recentFolders;

signals:
    void addressBarNavigated(const QString& path);
    void searchRequested(const QString& searchText);
    void recentFolderNavigated(const QString& path);

private slots:
    void onAddressBarEntered();
    void onSearchBarEntered();
    void onRecentFolderSelected();
};

#endif