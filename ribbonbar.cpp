#include "ribbonbar.h"
#include <QStyle>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QFrame>
#include <QApplication>
#include <QMenu>

RibbonBar::RibbonBar(QWidget *parent) : QWidget(parent)
{
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

QWidget *RibbonBar::createFileTab()
{
    QWidget *tab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(tab);

    QToolBar *toolbar = new QToolBar("File");
    toolbar->addAction("File");

    layout->addWidget(toolbar);
    tab->setLayout(layout);
    return tab;
}

QWidget *RibbonBar::createHomeTab()
{
    QWidget *tab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(tab);

    QToolBar *toolbar = new QToolBar("Home");
    toolbar->addAction("Home");

    layout->addWidget(toolbar);
    tab->setLayout(layout);
    return tab;
}

QWidget *RibbonBar::createShareTab()
{
    QWidget *tab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(tab);

    QToolBar *toolbar = new QToolBar("Share");
    toolbar->addAction("Share");

    layout->addWidget(toolbar);
    tab->setLayout(layout);
    return tab;
}

QWidget *RibbonBar::createViewTab()
{
    QWidget *tab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(5, 5, 5, 5);

    QWidget *viewGroup = new QWidget;
    QHBoxLayout *groupLayout = new QHBoxLayout(viewGroup);
    groupLayout->setSpacing(5);
    groupLayout->setContentsMargins(10, 0, 10, 0);

    QWidget *viewModeSection = new QWidget;
    QVBoxLayout *viewModeLayout = new QVBoxLayout(viewModeSection);
    viewModeLayout->setSpacing(2);
    viewModeLayout->setContentsMargins(0, 0, 0, 0);


    QWidget *viewModeButtons = new QWidget;
    QHBoxLayout *buttonsLayout = new QHBoxLayout(viewModeButtons);
    buttonsLayout->setSpacing(2);
    buttonsLayout->setContentsMargins(0, 0, 0, 0);

     
    QLabel *viewModeLabel = new QLabel("Layout");
    viewModeLabel->setAlignment(Qt::AlignCenter);

    
    QActionGroup *viewModeGroup = new QActionGroup(this);
    viewModeGroup->setExclusive(true);

    QAction *extraLargeIconsAction = new QAction(QIcon(":/icons/extra_large_icons.png"), "Extra large icons", this);
    extraLargeIconsAction->setCheckable(true);
    extraLargeIconsAction->setActionGroup(viewModeGroup);
    extraLargeIconsAction->setData(static_cast<int>(ViewMode::Icons));
    connect(extraLargeIconsAction, &QAction::triggered, this, [this]() { emit viewModeChanged(ViewMode::Icons); });

    QAction *largeIconsAction = new QAction(QIcon(":/icons/large_icons.png"), "Large icons", this);
    largeIconsAction->setCheckable(true);
    largeIconsAction->setActionGroup(viewModeGroup);
    largeIconsAction->setData(static_cast<int>(ViewMode::Icons));
    connect(largeIconsAction, &QAction::triggered, this, [this]() { emit viewModeChanged(ViewMode::Icons); });

    QAction *mediumIconsAction = new QAction(QIcon(":/icons/medium_icons.png"), "Medium icons", this);
    mediumIconsAction->setCheckable(true);
    mediumIconsAction->setActionGroup(viewModeGroup);
    mediumIconsAction->setData(static_cast<int>(ViewMode::Icons));
    mediumIconsAction->setChecked(true); 
    connect(mediumIconsAction, &QAction::triggered, this, [this]() { emit viewModeChanged(ViewMode::Icons); });

    QAction *smallIconsAction = new QAction(QIcon(":/icons/small_icons.png"), "Small icons", this);
    smallIconsAction->setCheckable(true);
    smallIconsAction->setActionGroup(viewModeGroup);
    smallIconsAction->setData(static_cast<int>(ViewMode::Icons));
    connect(smallIconsAction, &QAction::triggered, this, [this]() { emit viewModeChanged(ViewMode::Icons); });

    QAction *listAction = new QAction(QIcon(":/icons/list.png"), "List", this);
    listAction->setCheckable(true);
    listAction->setActionGroup(viewModeGroup);
    listAction->setData(static_cast<int>(ViewMode::List));
    connect(listAction, &QAction::triggered, this, [this]() { emit viewModeChanged(ViewMode::List); });

    QAction *detailsAction = new QAction(QIcon(":/icons/details.png"), "Details", this);
    detailsAction->setCheckable(true);
    detailsAction->setActionGroup(viewModeGroup);
    detailsAction->setData(static_cast<int>(ViewMode::Details));
    connect(detailsAction, &QAction::triggered, this, [this]() { emit viewModeChanged(ViewMode::Details); });

    QAction *tilesAction = new QAction(QIcon(":/icons/tiles.png"), "Tiles", this);
    tilesAction->setCheckable(true);
    tilesAction->setActionGroup(viewModeGroup);
    tilesAction->setData(static_cast<int>(ViewMode::Tiles));
    connect(tilesAction, &QAction::triggered, this, [this]() { emit viewModeChanged(ViewMode::Tiles); });

    QAction *contentAction = new QAction(QIcon(":/icons/content.png"), "Content", this);
    contentAction->setCheckable(true);
    contentAction->setActionGroup(viewModeGroup);
    contentAction->setData(static_cast<int>(ViewMode::Content));
    connect(contentAction, &QAction::triggered, this, [this]() { emit viewModeChanged(ViewMode::Content); });

    extraLargeIconsAction->setIcon(style()->standardIcon(QStyle::SP_FileDialogListView));
    largeIconsAction->setIcon(style()->standardIcon(QStyle::SP_FileDialogListView));
    mediumIconsAction->setIcon(style()->standardIcon(QStyle::SP_FileDialogListView));
    smallIconsAction->setIcon(style()->standardIcon(QStyle::SP_FileDialogListView));
    listAction->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    detailsAction->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    tilesAction->setIcon(style()->standardIcon(QStyle::SP_FileDialogContentsView));
    contentAction->setIcon(style()->standardIcon(QStyle::SP_FileDialogInfoView));

    QToolButton *extraLargeIconsButton = new QToolButton();
    extraLargeIconsButton->setDefaultAction(extraLargeIconsAction);
    extraLargeIconsButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    extraLargeIconsButton->setIconSize(QSize(32, 32));
    extraLargeIconsButton->setFixedSize(64, 50);

    QToolButton *largeIconsButton = new QToolButton();
    largeIconsButton->setDefaultAction(largeIconsAction);
    largeIconsButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    largeIconsButton->setIconSize(QSize(32, 32));
    largeIconsButton->setFixedSize(64, 50);

    QToolButton *mediumIconsButton = new QToolButton();
    mediumIconsButton->setDefaultAction(mediumIconsAction);
    mediumIconsButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    mediumIconsButton->setIconSize(QSize(32, 32));
    mediumIconsButton->setFixedSize(64, 50);

    QToolButton *smallIconsButton = new QToolButton();
    smallIconsButton->setDefaultAction(smallIconsAction);
    smallIconsButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    smallIconsButton->setIconSize(QSize(32, 32));
    smallIconsButton->setFixedSize(64, 50);

    QToolButton *listButton = new QToolButton();
    listButton->setDefaultAction(listAction);
    listButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    listButton->setIconSize(QSize(32, 32));
    listButton->setFixedSize(64, 50);

    QToolButton *detailsButton = new QToolButton();
    detailsButton->setDefaultAction(detailsAction);
    detailsButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    detailsButton->setIconSize(QSize(32, 32));
    detailsButton->setFixedSize(64, 50);

    QToolButton *tilesButton = new QToolButton();
    tilesButton->setDefaultAction(tilesAction);
    tilesButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    tilesButton->setIconSize(QSize(32, 32));
    tilesButton->setFixedSize(64, 50);

    QToolButton *contentButton = new QToolButton();
    contentButton->setDefaultAction(contentAction);
    contentButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    contentButton->setIconSize(QSize(32, 32));
    contentButton->setFixedSize(64, 50);

    buttonsLayout->addWidget(extraLargeIconsButton);
    buttonsLayout->addWidget(largeIconsButton);
    buttonsLayout->addWidget(mediumIconsButton);
    buttonsLayout->addWidget(smallIconsButton);
    buttonsLayout->addWidget(listButton);
    buttonsLayout->addWidget(detailsButton);
    buttonsLayout->addWidget(tilesButton);
    buttonsLayout->addWidget(contentButton);

    viewModeLayout->addWidget(viewModeButtons);
    viewModeSection->setLayout(viewModeLayout);
    viewModeLayout->addWidget(viewModeLabel);

    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::VLine);
    separator->setFrameShadow(QFrame::Sunken);

    groupLayout->addWidget(viewModeSection);
    groupLayout->addWidget(separator);

    groupLayout->addStretch();

    layout->addWidget(viewGroup);
    tab->setLayout(layout);
    return tab;
}