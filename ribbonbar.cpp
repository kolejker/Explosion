#include "ribbonbar.h"
#include <QStyle>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QFrame>
#include <QApplication>
#include <QMenu>

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
    
    QAction* backAction = toolbar->addAction(style()->standardIcon(QStyle::SP_ArrowBack), "Back");
    QAction* forwardAction = toolbar->addAction(style()->standardIcon(QStyle::SP_ArrowForward), "Forward");
    
    recentFoldersAction = new QAction(style()->standardIcon(QStyle::SP_ArrowDown), "Recent Folders", this);
    recentFoldersMenu = new QMenu(this);
    recentFoldersAction->setMenu(recentFoldersMenu);
    
    QToolButton* recentFoldersButton = new QToolButton();
    recentFoldersButton->setIcon(style()->standardIcon(QStyle::SP_ArrowDown));
    recentFoldersButton->setToolTip("Recent Folders");
    recentFoldersButton->setFixedWidth(20);  
    recentFoldersButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    recentFoldersButton->setMenu(recentFoldersMenu);
    recentFoldersButton->setPopupMode(QToolButton::InstantPopup);
    toolbar->addWidget(recentFoldersButton);
    
    
    QAction* upAction = toolbar->addAction(style()->standardIcon(QStyle::SP_ArrowUp), "Up");

    QWidget* addressSearchContainer = new QWidget();
    QHBoxLayout* addressSearchLayout = new QHBoxLayout(addressSearchContainer);
    addressSearchLayout->setContentsMargins(0, 0, 0, 0);
    addressSearchLayout->setSpacing(5);

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

    tabWidget->setStyleSheet(R"(
        QTabBar::tab {
            height: 22px;
            width: 55px;
            margin: 0px;
            border-top: 1px solid white;
            border-left: 1px solid white;
            border-right: 1px solid white;
            border-bottom: 1px solid gray; 
            background: white;
        }
        
        QTabBar::tab:selected {
            border-top: 1px solid gray;
            border-left: 1px solid gray;
            border-right: 1px solid gray;
            border-bottom: 1px solid white; 
            background: white;
            z-index: 2;
        }
        
        QTabBar::tab:!selected {
            background: white;
            color: black;
            border: 1px solid white; 
        }
        
        QTabBar::tab:first {
            background-color: #0078D7; 
            color: white;
            border: 1px solid #0078D7;
        }

        )");
        
        
        tabWidget->setCurrentIndex(1); 


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

void RibbonBar::updateRecentFolders(const QString& path) {
    if (!recentFolders.contains(path)) {
        recentFolders.prepend(path);
        
        while (recentFolders.size() > 10) {
            recentFolders.removeLast();
        }
        
        recentFoldersMenu->clear();
        
        for (const QString& folder : recentFolders) {
            QFileInfo fileInfo(folder);
            QAction* action = recentFoldersMenu->addAction(fileInfo.fileName());
            action->setData(folder);
            action->setToolTip(folder);
            connect(action, &QAction::triggered, this, &RibbonBar::onRecentFolderSelected);
        }
    }
}

void RibbonBar::onRecentFolderSelected() {
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        QString path = action->data().toString();
        emit recentFolderNavigated(path);
    }
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