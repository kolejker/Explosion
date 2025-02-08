#include <QApplication>
#include <QMainWindow>
#include <QTreeView>
#include <QListView>
#include <QFileSystemModel>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QHeaderView>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QStyle>
#include <QFileIconProvider>
#include <QStorageInfo>

class Explosion : public QMainWindow {
    Q_OBJECT
public:
    Explosion(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("File Explorer");
        setupUI();
        setupRibbon();
        setupToolbar();
        setupFileSystem();
        setupQuickAccess();
    }

private:
    QTreeView *navigationTree;
    QListView *contentView;
    QFileSystemModel *fileModel;
    QStandardItemModel *quickAccessModel;
    QTreeView *quickAccessTree;
    
    void setupUI() {
        QWidget *centralWidget = new QWidget(this);
        QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
        
        QSplitter *splitter = new QSplitter(Qt::Horizontal);

        QWidget *leftPanel = new QWidget;
        QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
        leftLayout->setContentsMargins(0, 0, 0, 0);
        
        quickAccessTree = new QTreeView;
        quickAccessTree->setHeaderHidden(true);
        quickAccessTree->setFrameShape(QFrame::NoFrame);
        quickAccessTree->setEditTriggers(QTreeView::NoEditTriggers);
        leftLayout->addWidget(quickAccessTree);

        contentView = new QListView;
        contentView->setViewMode(QListView::IconMode);
        contentView->setIconSize(QSize(32, 32));
        contentView->setGridSize(QSize(100, 80));
        contentView->setSpacing(10);
        contentView->setResizeMode(QListView::Adjust);
        contentView->setWrapping(true);
        
        splitter->addWidget(leftPanel);
        splitter->addWidget(contentView);
        splitter->setStretchFactor(1, 2);
        
        QList<int> sizes;
        sizes << 200 << 600;
        splitter->setSizes(sizes);
        
        mainLayout->addWidget(splitter);
        setCentralWidget(centralWidget);
        
        resize(1024, 768);
    }

    void setupRibbon () {
        QToolBar *ribbon = addToolBar("Ribbon");
        ribbon->setMovable(false);
        
        ribbon->addAction("fish?");

    }

    void setupQuickAccess() {
        quickAccessModel = new QStandardItemModel(this);
        QStandardItem *rootItem = quickAccessModel->invisibleRootItem();
        
        QStandardItem *quickAccess = new QStandardItem("Quick access");
        quickAccess->setIcon(style()->standardIcon(QStyle::SP_DirIcon));
        
        QList<QPair<QString, QString>> commonFolders = {
            {"Desktop", QDir::homePath() + "/Desktop"},
            {"Downloads", QDir::homePath() + "/Downloads"},
            {"Documents", QDir::homePath() + "/Documents"},
            {"Pictures", QDir::homePath() + "/Pictures"},
            {"Music", QDir::homePath() + "/Music"},
            {"Videos", QDir::homePath() + "/Videos"}
        };

        for (const auto &folder : commonFolders) {
            addQuickAccessItem(quickAccess, folder.first, folder.second,
                             style()->standardIcon(QStyle::SP_DirIcon));
        }
        rootItem->appendRow(quickAccess);
        
        QStandardItem *thisPC = new QStandardItem("This PC");
        thisPC->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));

        for (const auto &folder : commonFolders) {
            addQuickAccessItem(thisPC, folder.first, folder.second,
                             style()->standardIcon(QStyle::SP_DirIcon));
        }

        for (const QStorageInfo &storage : QStorageInfo::mountedVolumes()) {
            if (storage.isValid() && storage.isReady()) {
                QString name = QString("Local Disk (%1)").arg(storage.rootPath());
                addQuickAccessItem(thisPC, name, storage.rootPath(),
                                 style()->standardIcon(QStyle::SP_DriveHDIcon));
            }
        }
        
        rootItem->appendRow(thisPC);

        QStandardItem *network = new QStandardItem("Network");
        network->setIcon(style()->standardIcon(QStyle::SP_DriveNetIcon));
        rootItem->appendRow(network);
        
        quickAccessTree->setModel(quickAccessModel);
        quickAccessTree->expandAll();

        connect(quickAccessTree->selectionModel(), &QItemSelectionModel::selectionChanged,
                this, &Explosion::onNavigationSelection);
    }

    void addQuickAccessItem(QStandardItem *parent, const QString &name, 
                           const QString &path, const QIcon &icon) {
        QStandardItem *item = new QStandardItem(icon, name);
        item->setData(path, Qt::UserRole + 1);  
        parent->appendRow(item);
    }

    void setupToolbar() {
        QToolBar *toolbar = addToolBar("Navigation");
        toolbar->setMovable(false);
        
        toolbar->addAction(style()->standardIcon(QStyle::SP_ArrowBack), "Back");
        toolbar->addAction(style()->standardIcon(QStyle::SP_ArrowForward), "Forward");
        toolbar->addAction(style()->standardIcon(QStyle::SP_ArrowUp), "Up");
    }



    void setupFileSystem() {
        fileModel = new QFileSystemModel(this);
        fileModel->setRootPath(QDir::homePath());
        fileModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);
        
        contentView->setModel(fileModel);
        contentView->setRootIndex(fileModel->index(QDir::homePath()));
    }

private slots:
    void onNavigationSelection(const QItemSelection &selected, const QItemSelection &) {
        if (selected.indexes().isEmpty())
            return;
            
        QModelIndex index = selected.indexes().first();
        QString path = index.data(Qt::UserRole + 1).toString();
        
        if (!path.isEmpty()) {
            QFileInfo fileInfo(path);
            if (fileInfo.exists() && fileInfo.isDir()) {
                contentView->setRootIndex(fileModel->setRootPath(path));
                statusBar()->showMessage("Location: " + path);
            }
        }
    }
};

#include "main.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setStyle("Fusion");
    
    Explosion explorer;
    explorer.show();
    return app.exec();
}