#include "fileviewmodel.h"
#include <QHeaderView>
#include <QDateTime>

FileViewModel::FileViewModel(QObject* parent)
    : QObject(parent), fileModel(nullptr), viewContainer(nullptr), 
    iconView(nullptr), listView(nullptr), detailsView(nullptr), 
    tilesView(nullptr), contentView(nullptr), currentMode(ViewMode::Icons) {
    
    fileModel = new QFileSystemModel(this);
    fileModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);
    
    fileModel->setReadOnly(false);
}

FileViewModel::~FileViewModel() {

}

void FileViewModel::setupFileSystem(QStackedWidget* container) {
    viewContainer = container;
    if (!viewContainer) return;
    
    iconView = new QListView(viewContainer);
    listView = new QListView(viewContainer);
    detailsView = new QTableView(viewContainer);
    tilesView = new QListView(viewContainer);
    contentView = new QListView(viewContainer);
    
    configureIconView();
    configureListView();
    configureDetailsView();
    configureTilesView();
    configureContentView();
    
    viewContainer->addWidget(iconView);
    viewContainer->addWidget(listView);
    viewContainer->addWidget(detailsView);
    viewContainer->addWidget(tilesView);
    viewContainer->addWidget(contentView);
    
    iconView->setModel(fileModel);
    listView->setModel(fileModel);
    detailsView->setModel(fileModel);
    tilesView->setModel(fileModel);
    contentView->setModel(fileModel);
    
    iconView->setItemDelegate(new IconViewDelegate(this));
    tilesView->setItemDelegate(new TilesViewDelegate(this));
    contentView->setItemDelegate(new ContentViewDelegate(this));
    
    setViewMode(ViewMode::Icons);
}

void FileViewModel::setRootPath(const QString& path) {
    if (!QFileInfo::exists(path)) return;
    
    rootPath = path;
    QModelIndex index = fileModel->setRootPath(path);
    
    updateCurrentViewRoot();
}

QString FileViewModel::currentRootPath() const {
    return rootPath;
}

QModelIndex FileViewModel::indexForPath(const QString& path) const {
    return fileModel->index(path);
}

void FileViewModel::setViewMode(ViewMode mode) {
    if (!viewContainer) return;
    
    currentMode = mode;
    
    switch (mode) {
        case ViewMode::Icons:
            viewContainer->setCurrentWidget(iconView);
            break;
        case ViewMode::List:
            viewContainer->setCurrentWidget(listView);
            break;
        case ViewMode::Details:
            viewContainer->setCurrentWidget(detailsView);
            break;
        case ViewMode::Tiles:
            viewContainer->setCurrentWidget(tilesView);
            break;
        case ViewMode::Content:
            viewContainer->setCurrentWidget(contentView);
            break;
    }
    
    updateCurrentViewRoot();
}

void FileViewModel::configureIconView() {
    iconView->setViewMode(QListView::IconMode);
    iconView->setIconSize(QSize(48, 48));
    iconView->setGridSize(QSize(120, 100));
    iconView->setSpacing(10);
    iconView->setResizeMode(QListView::Adjust);
    iconView->setWrapping(true);
    iconView->setUniformItemSizes(true);
    
    connect(iconView, &QListView::doubleClicked, this, &FileViewModel::onItemDoubleClicked);
}

void FileViewModel::configureListView() {
    listView->setViewMode(QListView::ListMode);
    listView->setIconSize(QSize(16, 16));
    listView->setSpacing(2);
    listView->setUniformItemSizes(true);
    
    listView->setFlow(QListView::TopToBottom);
    listView->setWrapping(true);
    listView->setResizeMode(QListView::Adjust);
    
    listView->setItemDelegate(new ListViewDelegate(this));
    
    listView->setGridSize(QSize(200, 20));
    
    connect(listView, &QListView::doubleClicked, this, &FileViewModel::onItemDoubleClicked);
}


void FileViewModel::configureDetailsView() {
    detailsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    detailsView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    detailsView->setSortingEnabled(true);
    detailsView->verticalHeader()->setVisible(false);
    
    for (int i = 1; i < fileModel->columnCount(); ++i) {
        detailsView->setColumnWidth(i, 120);  
    }
    
    detailsView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    
    detailsView->verticalHeader()->setDefaultSectionSize(24);
    
    connect(detailsView, &QTableView::doubleClicked, this, &FileViewModel::onItemDoubleClicked);
}

void FileViewModel::configureTilesView() {
    tilesView->setViewMode(QListView::ListMode);
    tilesView->setIconSize(QSize(32, 32));
    tilesView->setGridSize(QSize(300, 44)); 
    tilesView->setSpacing(2);
    tilesView->setResizeMode(QListView::Adjust);
    
    tilesView->setFlow(QListView::TopToBottom);
    tilesView->setWrapping(true);
    tilesView->setUniformItemSizes(true);
    
    connect(tilesView, &QListView::doubleClicked, this, &FileViewModel::onItemDoubleClicked);
}

void FileViewModel::configureContentView() {
    contentView->setViewMode(QListView::ListMode);
    contentView->setIconSize(QSize(48, 48)); 
    contentView->setGridSize(QSize(400, 58)); 
    contentView->setSpacing(1);
    contentView->setUniformItemSizes(true);
    
    connect(contentView, &QListView::doubleClicked, this, &FileViewModel::onItemDoubleClicked);
}

void FileViewModel::onItemDoubleClicked(const QModelIndex &index) {
    emit itemActivated(index);
}

void FileViewModel::updateCurrentViewRoot() {
    if (rootPath.isEmpty()) return;
    
    QModelIndex index = fileModel->index(rootPath);
    
    iconView->setRootIndex(index);
    listView->setRootIndex(index);
    detailsView->setRootIndex(index);
    tilesView->setRootIndex(index);
    contentView->setRootIndex(index);
}

void FileViewModel::applySearchFilter(const QStringList& filters, bool hideNonMatching) {
    if (filters.isEmpty()) {
        clearFilters();
        return;
    }
    
    fileModel->setNameFilters(filters);
    
    fileModel->setNameFilterDisables(!hideNonMatching);
    
    QDir::Filters currentFilters = fileModel->filter();
    if (!(currentFilters & QDir::AllDirs)) {
        fileModel->setFilter(currentFilters | QDir::AllDirs);
    }
}

void FileViewModel::clearFilters() {
    fileModel->setNameFilters(QStringList());
    fileModel->setNameFilterDisables(true);
}

IconViewDelegate::IconViewDelegate(QObject *parent) : QStyledItemDelegate(parent) {
}

TilesViewDelegate::TilesViewDelegate(QObject *parent) : QStyledItemDelegate(parent) {
}

void TilesViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (!index.isValid()) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }
    
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    
    if (opt.state & QStyle::State_Selected) {
        painter->fillRect(opt.rect, opt.palette.highlight());
    }
    
    QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
    QString text = index.data(Qt::DisplayRole).toString();
    
    QRect iconRect = opt.rect;
    iconRect.setSize(QSize(32, 32));
    iconRect.moveTop(opt.rect.top() + (opt.rect.height() - 32) / 2);
    iconRect.moveLeft(opt.rect.left() + 4);
    icon.paint(painter, iconRect);
    
    if (opt.state & QStyle::State_Selected) {
        painter->setPen(opt.palette.color(QPalette::HighlightedText));
    } else {
        painter->setPen(opt.palette.color(QPalette::Text));
    }
    
    QRect textRect = opt.rect;
    textRect.setLeft(iconRect.right() + 8);
    textRect.setTop(opt.rect.top() + 2);
    textRect.setHeight(opt.rect.height());
    
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, text);
}

QSize TilesViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(44); 
    return size;
}

ContentViewDelegate::ContentViewDelegate(QObject *parent) : QStyledItemDelegate(parent) {
}

void ContentViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (!index.isValid()) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }
    
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    
    if (opt.state & QStyle::State_Selected) {
        painter->fillRect(opt.rect, opt.palette.highlight());
    }
    
    QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
    QString text = index.data(Qt::DisplayRole).toString();
    
    QFileSystemModel *model = qobject_cast<QFileSystemModel*>(const_cast<QAbstractItemModel*>(index.model()));
    QString dateModified;
    if (model) {
        QFileInfo fileInfo = model->fileInfo(index);
        dateModified = fileInfo.lastModified().toString("M/d/yyyy h:mm AP");
    }
    
    QRect iconRect = opt.rect;
    iconRect.setSize(QSize(48, 48));
    iconRect.moveTop(opt.rect.top() + (opt.rect.height() - 48) / 2);
    iconRect.moveLeft(opt.rect.left() + 4);
    icon.paint(painter, iconRect);
    
    if (opt.state & QStyle::State_Selected) {
        painter->setPen(opt.palette.color(QPalette::HighlightedText));
    } else {
        painter->setPen(opt.palette.color(QPalette::Text));
    }
    
    QRect textRect = opt.rect;
    textRect.setLeft(iconRect.right() + 8);
    textRect.setTop(opt.rect.top() + 10);
    textRect.setHeight(20);
    textRect.setRight(opt.rect.right() - 150); 
    
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, text);
    
    QRect dateRect = opt.rect;
    dateRect.setLeft(opt.rect.right() - 140);
    dateRect.setTop(opt.rect.top() + 10);
    dateRect.setHeight(20);
    
    painter->drawText(dateRect, Qt::AlignVCenter | Qt::AlignRight, "Date modified: " + dateModified);
}

QSize ContentViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(58); 
    return size;
}

ListViewDelegate::ListViewDelegate(QObject *parent) : QStyledItemDelegate(parent) {
}

void ListViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (!index.isValid()) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }
    
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    
    if (opt.state & QStyle::State_Selected) {
        painter->fillRect(opt.rect, opt.palette.highlight());
    }
    
    QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
    QString text = index.data(Qt::DisplayRole).toString();
    
    QRect iconRect = opt.rect;
    iconRect.setSize(QSize(16, 16));
    iconRect.moveTop(opt.rect.top() + (opt.rect.height() - 16) / 2);
    iconRect.moveLeft(opt.rect.left() + 4);
    icon.paint(painter, iconRect);
    
    if (opt.state & QStyle::State_Selected) {
        painter->setPen(opt.palette.color(QPalette::HighlightedText));
    } else {
        painter->setPen(opt.palette.color(QPalette::Text));
    }
    
    QRect textRect = opt.rect;
    textRect.setLeft(iconRect.right() + 6);
    textRect.setTop(opt.rect.top());
    textRect.setHeight(opt.rect.height());
    
    QString displayText = painter->fontMetrics().elidedText(text, Qt::ElideRight, textRect.width() - 5);
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, displayText);
}

QSize ListViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(20); 
    return size;
}