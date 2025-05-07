#include "viewtab.h"

ViewTab::ViewTab(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
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

    viewModeGroup = new QActionGroup(this);
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

    buttonsLayout->addWidget(createViewModeButton(extraLargeIconsAction));
    buttonsLayout->addWidget(createViewModeButton(largeIconsAction));
    buttonsLayout->addWidget(createViewModeButton(mediumIconsAction));
    buttonsLayout->addWidget(createViewModeButton(smallIconsAction));
    buttonsLayout->addWidget(createViewModeButton(listAction));
    buttonsLayout->addWidget(createViewModeButton(detailsAction));
    buttonsLayout->addWidget(createViewModeButton(tilesAction));
    buttonsLayout->addWidget(createViewModeButton(contentAction));

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
    setLayout(layout);
}

QToolButton* ViewTab::createViewModeButton(QAction* action) {
    QToolButton* button = new QToolButton();
    button->setDefaultAction(action);
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setIconSize(QSize(32, 32));
    button->setFixedSize(64, 50);
    return button;
}