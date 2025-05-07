#include "ribbonbar.h"
#include <QStyle>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QFrame>
#include <QApplication>

#include "fileviewmodel.h"
#include "searchmanager.h"
#include "tabs/filetab.h"
#include "tabs/hometab.h"
#include "tabs/sharetab.h"
#include "tabs/viewtab.h"

RibbonBar::RibbonBar(QWidget *parent) : QWidget(parent)
{
    setFixedHeight(155);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    tabWidget = new QTabWidget(this);
    tabWidget->setTabPosition(QTabWidget::North);
    
    fileTab = new FileTab(this);
    homeTab = new HomeTab(this);
    shareTab = new ShareTab(this);
    viewTab = new ViewTab(this);
    
    tabWidget->addTab(fileTab, "File");
    tabWidget->addTab(homeTab, "Home");
    tabWidget->addTab(shareTab, "Share");
    tabWidget->addTab(viewTab, "View");
    
    connect(viewTab, &ViewTab::viewModeChanged, this, &RibbonBar::viewModeChanged);

    QToolBar *toolbar = new QToolBar("Navigation");
    toolbar->setMovable(false);
    toolbar->setStyleSheet("QToolBar { padding-top: -3px; spacing: 0px; padding-left: 4px; }");
    
    QAction *backAction = toolbar->addAction(style()->standardIcon(QStyle::SP_ArrowBack), "Back");
    QToolButton *backButton = qobject_cast<QToolButton *>(toolbar->widgetForAction(backAction));
    if (backButton)
    {
        backButton->setFixedWidth(30); 
    }

    QAction *forwardAction = toolbar->addAction(style()->standardIcon(QStyle::SP_ArrowForward), "Forward");
    QToolButton *forwardButton = qobject_cast<QToolButton *>(toolbar->widgetForAction(forwardAction));
    if (forwardButton)
    {
        forwardButton->setFixedWidth(30); 
    }

    recentFoldersAction = new QAction(style()->standardIcon(QStyle::SP_ArrowDown), "Recent Folders", this);
    recentFoldersMenu = new QMenu(this);
    recentFoldersAction->setMenu(recentFoldersMenu);

    QToolButton *recentFoldersButton = new QToolButton();
    recentFoldersButton->setIcon(style()->standardIcon(QStyle::SP_ArrowDown));
    recentFoldersButton->setToolTip("Recent Folders");
    recentFoldersButton->setFixedWidth(20); 
    recentFoldersButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    recentFoldersButton->setMenu(recentFoldersMenu);
    recentFoldersButton->setPopupMode(QToolButton::InstantPopup);
    toolbar->addWidget(recentFoldersButton);

    QAction *upAction = toolbar->addAction(style()->standardIcon(QStyle::SP_ArrowUp), "Up");
    QToolButton *upButton = qobject_cast<QToolButton *>(toolbar->widgetForAction(upAction));
    if (upButton)
    {
        upButton->setFixedWidth(24); 
    }

    QWidget *spacer = new QWidget();
    spacer->setFixedWidth(1);
    toolbar->addWidget(spacer);

    QWidget *addressSearchContainer = new QWidget();
    QHBoxLayout *addressSearchLayout = new QHBoxLayout(addressSearchContainer);
    addressSearchLayout->setContentsMargins(0, 0, 0, 0);

    addressBar = new QLineEdit();
    addressBar->setMinimumWidth(226);
    addressBar->setClearButtonEnabled(true);
    addressBar->setPlaceholderText("Address");

    searchBar = new QLineEdit();
    searchBar->setMinimumWidth(200);
    searchBar->setMaximumWidth(200);
    searchBar->setClearButtonEnabled(true);
    searchBar->setPlaceholderText("Search");

    QString lineEditStyle = R"(
        QLineEdit {
            height: 18px;
            border-radius: 0px;
            border: 1px solid gray;
        }
    )";

    addressBar->setStyleSheet(lineEditStyle);
    searchBar->setStyleSheet(lineEditStyle);

    QAction *searchAction = new QAction(style()->standardIcon(QStyle::SP_FileDialogContentsView), "", this);
    searchBar->addAction(searchAction, QLineEdit::LeadingPosition);

    addressSearchLayout->addWidget(addressBar);
    addressSearchLayout->addSpacing(12);
    addressSearchLayout->addWidget(searchBar);
    addressSearchLayout->addSpacing(12);

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

void RibbonBar::onAddressBarEntered()
{
    QString path = addressBar->text();
    emit addressBarNavigated(path);
}

void RibbonBar::onSearchBarEntered()
{
    QString searchText = searchBar->text();
    emit searchRequested(searchText);
}

void RibbonBar::updateRecentFolders(const QString &path)
{
    if (!recentFolders.contains(path))
    {
        recentFolders.prepend(path);

        while (recentFolders.size() > 10)
        {
            recentFolders.removeLast();
        }

        recentFoldersMenu->clear();

        for (const QString &folder : recentFolders)
        {
            QFileInfo fileInfo(folder);
            QAction *action = recentFoldersMenu->addAction(fileInfo.fileName());
            action->setData(folder);
            action->setToolTip(folder);
            connect(action, &QAction::triggered, this, &RibbonBar::onRecentFolderSelected);
        }
    }
}

void RibbonBar::onRecentFolderSelected()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        QString path = action->data().toString();
        emit recentFolderNavigated(path);
    }
}