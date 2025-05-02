#include "fileviewmodel.h"

FileViewModel::FileViewModel(QObject* parent)
    : QObject(parent), fileModel(nullptr), contentView(nullptr) {
    fileModel = new QFileSystemModel(this);

    fileModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);
}

FileViewModel::~FileViewModel() {

}

void FileViewModel::setupFileSystem(QListView* view) {
    contentView = view;
    if (contentView) {
        contentView->setModel(fileModel);
        setViewMode(ViewMode::Icons); 
    }
}

void FileViewModel::setRootPath(const QString& path) {
    if (!QFileInfo::exists(path)) return;
    
    rootPath = path;
    QModelIndex index = fileModel->setRootPath(path);
    
    if (contentView) {
        contentView->setRootIndex(index);
    }
}

QString FileViewModel::currentRootPath() const {
    return rootPath;
}

QModelIndex FileViewModel::indexForPath(const QString& path) const {
    return fileModel->index(path);
}

void FileViewModel::setViewMode(ViewMode mode) {
    if (!contentView) return;
    
    switch (mode) {
        case ViewMode::Icons:
            configureIconView();
            break;
        case ViewMode::List:
            configureListView();
            break;
        case ViewMode::Details:
            configureDetailsView();
            break;
        case ViewMode::Tiles:
            configureTilesView();
            break;
        case ViewMode::Content:
            configureContentView();
            break;
    }
}

void FileViewModel::configureIconView() {
    contentView->setViewMode(QListView::IconMode);
    contentView->setIconSize(QSize(48, 48));
    contentView->setGridSize(QSize(120, 100));
    contentView->setSpacing(10);
    contentView->setResizeMode(QListView::Adjust);
    contentView->setWrapping(true);
    contentView->setUniformItemSizes(true);
}

void FileViewModel::configureListView() {
    contentView->setViewMode(QListView::ListMode);
    contentView->setIconSize(QSize(16, 16));
    contentView->setSpacing(2);
    contentView->setUniformItemSizes(true);
}

void FileViewModel::configureDetailsView() {
    contentView->setViewMode(QListView::ListMode);
    contentView->setIconSize(QSize(16, 16));
    contentView->setSpacing(1);
    contentView->setUniformItemSizes(true);
}

void FileViewModel::configureTilesView() {
    contentView->setViewMode(QListView::IconMode);
    contentView->setIconSize(QSize(32, 32));
    contentView->setGridSize(QSize(200, 64));
    contentView->setSpacing(5);
    contentView->setResizeMode(QListView::Adjust);
    contentView->setWrapping(true);
    contentView->setUniformItemSizes(true);
}

void FileViewModel::configureContentView() {
    contentView->setViewMode(QListView::ListMode);
    contentView->setIconSize(QSize(16, 16));
    contentView->setSpacing(1);
    contentView->setUniformItemSizes(true);
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