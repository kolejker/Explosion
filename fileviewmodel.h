#ifndef FILEVIEWMODEL_H
#define FILEVIEWMODEL_H

#include <QFileSystemModel>
#include <QListView>
#include <QTreeView>
#include <QWidget>
#include <QIcon>
#include <QSize>
#include <QModelIndex>

enum class ViewMode {
    Icons,
    List,
    Details,
    Tiles,
    Content
};

class FileViewModel : public QObject {
    Q_OBJECT
public:
    explicit FileViewModel(QObject* parent = nullptr);
    ~FileViewModel();

    void setupFileSystem(QListView* contentView);
    
    void setRootPath(const QString& path);
    
    QString currentRootPath() const;
    
    void setViewMode(ViewMode mode);
    
    QModelIndex indexForPath(const QString& path) const;
    
    QFileSystemModel* model() const { return fileModel; }
    
    void applySearchFilter(const QStringList& filters, bool hideNonMatching);
    
    void clearFilters();

private:
    QFileSystemModel* fileModel;
    QListView* contentView;
    QString rootPath;
    
    void configureIconView();
    void configureListView();
    void configureDetailsView();
    void configureTilesView();
    void configureContentView();
};

#endif 