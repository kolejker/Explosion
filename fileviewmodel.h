#ifndef FILEVIEWMODEL_H
#define FILEVIEWMODEL_H

#include <QFileSystemModel>
#include <QListView>
#include <QTreeView>
#include <QTableView>
#include <QWidget>
#include <QIcon>
#include <QSize>
#include <QModelIndex>
#include <QStackedWidget>
#include <QStyledItemDelegate>
#include <QPainter>

enum class ViewMode {
    Icons,
    List,
    Details,
    Tiles,
    Content
};

class IconViewDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit IconViewDelegate(QObject *parent = nullptr);
};

class ListViewDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit ListViewDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

class TilesViewDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit TilesViewDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

class ContentViewDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit ContentViewDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

class ResizableStackedWidget : public QStackedWidget {
    Q_OBJECT
public:
    explicit ResizableStackedWidget(QWidget* parent = nullptr) : QStackedWidget(parent) {}

signals:
    void resized();

protected:
    void resizeEvent(QResizeEvent* event) override {
        QStackedWidget::resizeEvent(event);
        emit resized();
    }
};

class FileViewModel : public QObject {
    Q_OBJECT
public:
    explicit FileViewModel(QObject* parent = nullptr);
    ~FileViewModel();

    void setupFileSystem(QStackedWidget* viewContainer);
    
    void setRootPath(const QString& path);
    
    QString currentRootPath() const;
    
    void setViewMode(ViewMode mode);
    
    QModelIndex indexForPath(const QString& path) const;
    
    QFileSystemModel* model() const { return fileModel; }
    
    void applySearchFilter(const QStringList& filters, bool hideNonMatching);
    
    void clearFilters();

    void onContainerResized();

signals:
    void itemActivated(const QModelIndex& index);

private slots:
    void onItemDoubleClicked(const QModelIndex& index);

private:

  struct ColumnSizeConstraints {
        int minWidth;
        int maxWidth;
        int defaultWidth;
    };
    
    QMap<int, ColumnSizeConstraints> columnConstraints;
    
    void initializeColumnConstraints();
    void enforceColumnConstraints(int logicalIndex, int newSize);
    void redistributeColumnSpace();
    void ensureColumnsWithinView();
    QFileSystemModel* fileModel;
    QStackedWidget* viewContainer;
    QListView* iconView;
    QListView* listView;
    QTableView* detailsView;
    QListView* tilesView;
    QListView* contentView;
    QString rootPath;
    ViewMode currentMode;
    
    void onDetailsSectionResized(int logicalIndex, int oldSize, int newSize);
    void adjustDetailsColumnsToFit();
    void configureIconView();
    void configureListView();
    void configureDetailsView();
    void configureTilesView();
    void configureContentView();
    void updateCurrentViewRoot();

};

#endif