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
#include "fileviewmodel.h"

class AdvancedSearchDialog;

class RibbonBar : public QWidget {
    Q_OBJECT
public:
    explicit RibbonBar(QWidget *parent = nullptr);
    QLineEdit* getAddressBar() const { return addressBar; }
    QLineEdit* getSearchBar() const { return searchBar; }

private:
    QWidget *createFileTab();
    QWidget *createHomeTab();
    QWidget *createShareTab();
    QWidget *createViewTab();
    QLineEdit* addressBar;
    QLineEdit* searchBar;

signals:
    void addressBarNavigated(const QString& path);
    void searchRequested(const QString& searchText);

private slots:
    void onAddressBarEntered();
    void onSearchBarEntered();
};

#endif
