#ifndef RIBBONBAR_H
#define RIBBONBAR_H

#include <QTabWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QLineEdit>
#include <QMenu>
#include <QToolButton>
#include <QStringList>
#include <QAction>
#include "fileviewmodel.h"
#include "tabs/filetab.h"
#include "tabs/hometab.h"
#include "tabs/sharetab.h"
#include "tabs/viewtab.h"

class RibbonBar : public QWidget {
    Q_OBJECT
public:
    explicit RibbonBar(QWidget *parent = nullptr);
    QLineEdit* getAddressBar() const { return addressBar; }
    QLineEdit* getSearchBar() const { return searchBar; }
    void updateRecentFolders(const QString& path);

private:
    QTabWidget *tabWidget;
    QLineEdit* addressBar;
    QLineEdit* searchBar;
    QAction* recentFoldersAction;
    QMenu* recentFoldersMenu;
    QStringList recentFolders;
    
    FileTab* fileTab;
    HomeTab* homeTab;
    ShareTab* shareTab;
    ViewTab* viewTab;

signals:
    void addressBarNavigated(const QString& path);
    void searchRequested(const QString& searchText);
    void recentFolderNavigated(const QString& path);
    void viewModeChanged(ViewMode mode);

private slots:
    void onAddressBarEntered();
    void onSearchBarEntered();
    void onRecentFolderSelected();
};

#endif