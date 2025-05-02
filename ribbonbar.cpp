#include "ribbonbar.h"
#include <QStyle>
#include <QPushButton>
#include <QLabel>

#include "ribbonbar.h"
#include <QStyle>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QFrame>
#include <QApplication>

RibbonBar::RibbonBar(QWidget *parent): QWidget(parent) {
    setFixedHeight(155);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    QTabWidget *tabWidget = new QTabWidget(this);
    tabWidget->setTabPosition(QTabWidget::North);
    tabWidget->addTab(createFileTab(), "File");
    tabWidget->addTab(createHomeTab(), "Home");
    tabWidget->addTab(createShareTab(), "Share");
    tabWidget->addTab(createViewTab(), "View");

    QToolBar *toolbar = new QToolBar("Navigation");
    toolbar->setMovable(false);
    toolbar->addAction(style()->standardIcon(QStyle::SP_ArrowBack), "Back");
    toolbar->addAction(style()->standardIcon(QStyle::SP_ArrowForward), "Forward");
    toolbar->addAction(style()->standardIcon(QStyle::SP_ArrowUp), "Up");

    
    QWidget* addressSearchContainer = new QWidget();
    QHBoxLayout* addressSearchLayout = new QHBoxLayout(addressSearchContainer);
    addressSearchLayout->setContentsMargins(0, 0, 0, 0);
    addressSearchLayout->setSpacing(5);

    tabWidget->setStyleSheet(
        "QTabBar::tab {"
        "    height: 22px;"
        "width: 55px;"
        "    margin: 0px;"   
  "border: 1px solid gray"
        "}"
        "QTabBar::tab:selected, QTabBar::tab:!selected {"
        "    height: 22px;"       
        "}"
    );

    addressBar = new QLineEdit();
    addressBar->setMinimumWidth(300);
    addressBar->setClearButtonEnabled(true);
    addressBar->setPlaceholderText("Address");

    searchBar = new QLineEdit();
    searchBar->setMinimumWidth(150);
    searchBar->setMaximumWidth(200);
    searchBar->setClearButtonEnabled(true);
    searchBar->setPlaceholderText("Search");

    QAction* searchAction = new QAction(style()->standardIcon(QStyle::SP_FileDialogContentsView), "", this);
    searchBar->addAction(searchAction, QLineEdit::LeadingPosition);

    addressSearchLayout->addWidget(addressBar, 7);
    addressSearchLayout->addWidget(searchBar, 3);

    toolbar->addWidget(addressSearchContainer);

    connect(addressBar, &QLineEdit::returnPressed, this, &RibbonBar::onAddressBarEntered);
    connect(searchBar, &QLineEdit::returnPressed, this, &RibbonBar::onSearchBarEntered);

    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(toolbar);
    setLayout(mainLayout);
}

void RibbonBar::onAddressBarEntered() {
    QString path = addressBar->text();
    emit addressBarNavigated(path);
}

void RibbonBar::onSearchBarEntered() {
    QString searchText = searchBar->text();
    emit searchRequested(searchText);
}


QWidget *RibbonBar::createFileTab() {
    QWidget *tab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(tab);

    QToolBar *toolbar = new QToolBar("File");
    toolbar->addAction("File");

    layout->addWidget(toolbar);
    tab->setLayout(layout);
    return tab;
}

QWidget *RibbonBar::createHomeTab() {
    QWidget *tab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(tab);

    QToolBar *toolbar = new QToolBar("Home");
    toolbar->addAction("Home");

    layout->addWidget(toolbar);
    tab->setLayout(layout);
    return tab;
}

QWidget *RibbonBar::createShareTab() {
    QWidget *tab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(tab);

    QToolBar *toolbar = new QToolBar("Share");
    toolbar->addAction("Share");

    layout->addWidget(toolbar);
    tab->setLayout(layout);
    return tab;
}


QWidget *RibbonBar::createViewTab() {
    QWidget *tab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(tab);

    QToolBar *toolbar = new QToolBar("View");
    toolbar->addAction("View");

    layout->addWidget(toolbar);
    tab->setLayout(layout);
    return tab;
}
