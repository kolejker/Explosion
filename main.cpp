#include <QApplication>
#include <QMainWindow>
#include <QTreeView>
#include <QListView>
#include <QFileSystemModel>
#include <QWheelEvent>
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
#include <QStack>
#include <QDesktopServices>
#include <QUrl>
#include <QLineEdit>

#include "ribbonbar.h"

class Explosion: public QMainWindow {
    Q_OBJECT
public:
    Explosion(QWidget* parent = nullptr): QMainWindow(parent) {
        setWindowTitle("File Explorer");
        setupUI();
        setupFileSystem();
        setupQuickAccess();
        currentPath = QDir::homePath();
        updateAddressBar(currentPath);
    }

private:
    QTreeView* navigationTree;
    QListView* contentView;
    QFileSystemModel* fileModel;
    QStandardItemModel* quickAccessModel;
    QTreeView* quickAccessTree;
    QStack<QString> backStack;
    QStack<QString> forwardStack;
    QString currentPath;
    RibbonBar* ribbon;
    QLineEdit* addressBar;

    void setupUI() {
        QWidget* centralWidget = new QWidget(this);
        QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setContentsMargins(0, 0, 0, 0);

        ribbon = new RibbonBar(this);
        addressBar = ribbon->getAddressBar();
        connect(ribbon, &RibbonBar::addressBarNavigated, this, &Explosion::addressBarNavigateRequested);
        
        mainLayout->addWidget(ribbon);

        QSplitter* splitter = new QSplitter(Qt::Horizontal);

        QWidget* leftPanel = new QWidget;
        QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
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

        QToolBar* navToolBar = findChild<QToolBar*>();
        if (navToolBar) {
            QAction* backAction = navToolBar->actions()[0];
            QAction* forwardAction = navToolBar->actions()[1];
            QAction* upAction = navToolBar->actions()[2];
            
            connect(backAction, &QAction::triggered, this, &Explosion::navigateBack);
            connect(forwardAction, &QAction::triggered, this, &Explosion::navigateForward);
            connect(upAction, &QAction::triggered, this, &Explosion::navigateUp);
            
            backAction->setEnabled(false);
            forwardAction->setEnabled(false);
        }

        resize(1024, 768);
    }

    void setupQuickAccess() {
        quickAccessModel = new QStandardItemModel(this);
        QStandardItem* rootItem = quickAccessModel->invisibleRootItem();

        QStandardItem* quickAccess = new QStandardItem("Quick access");
        quickAccess->setIcon(style()->standardIcon(QStyle::SP_DirIcon));

        QList<QPair<QString, QString>> commonFolders = {
            {"Desktop", QDir::homePath() + "/Desktop"},
            {"Downloads", QDir::homePath() + "/Downloads"},
            {"Documents", QDir::homePath() + "/Documents"},
            {"Pictures", QDir::homePath() + "/Pictures"},
            {"Music", QDir::homePath() + "/Music"},
            {"Videos", QDir::homePath() + "/Videos"}
        };

        for (const auto& folder: commonFolders) {
            addQuickAccessItem(quickAccess, folder.first, folder.second,
                style()->standardIcon(QStyle::SP_DirIcon));
        }
        rootItem->appendRow(quickAccess);

        QStandardItem* thisPC = new QStandardItem("This PC");
        thisPC->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));

        for (const auto& folder: commonFolders) {
            addQuickAccessItem(thisPC, folder.first, folder.second,
                style()->standardIcon(QStyle::SP_DirIcon));
        }

        for (const QStorageInfo& storage: QStorageInfo::mountedVolumes()) {
            if (storage.isValid() && storage.isReady()) {
                QString name = QString("Local Disk (%1)").arg(storage.rootPath());
                addQuickAccessItem(thisPC, name, storage.rootPath(),
                    style()->standardIcon(QStyle::SP_DriveHDIcon));
            }
        }

        rootItem->appendRow(thisPC);

        QStandardItem* network = new QStandardItem("Network");
        network->setIcon(style()->standardIcon(QStyle::SP_DriveNetIcon));
        rootItem->appendRow(network);

        quickAccessTree->setModel(quickAccessModel);
        quickAccessTree->expandAll();

        connect(quickAccessTree->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &Explosion::onNavigationSelection);
    }

    void addQuickAccessItem(QStandardItem* parent,
        const QString& name,
        const QString& path,
        const QIcon& icon) {
        QStandardItem* item = new QStandardItem(icon, name);
        item->setData(path, Qt::UserRole + 1);
        parent->appendRow(item);
    }

    void setupFileSystem() {
        fileModel = new QFileSystemModel(this);
        fileModel->setRootPath(QDir::homePath());
        fileModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);

        connect(contentView, &QListView::doubleClicked, this, &Explosion::onItemActivated);

        contentView->setModel(fileModel);
        contentView->setRootIndex(fileModel->index(QDir::homePath()));
    }

    void updateAddressBar(const QString& path) {
        if (addressBar) {
            addressBar->setText(path);
        }
    }

    void navigateToPath(const QString& path, bool addToHistory = true) {
        if (!QFileInfo::exists(path)) return;
        
        if (addToHistory && !currentPath.isEmpty()) {
            backStack.push(currentPath);
            forwardStack.clear();
        }
        
        currentPath = path;
        contentView->setRootIndex(fileModel->setRootPath(path));
        statusBar()->showMessage("Location: " + path);
        updateAddressBar(path);

        QToolBar* navToolBar = findChild<QToolBar*>();
        if (navToolBar) {
            navToolBar->actions()[0]->setEnabled(!backStack.isEmpty());
            navToolBar->actions()[1]->setEnabled(!forwardStack.isEmpty());
            navToolBar->actions()[2]->setEnabled(QDir(path).cdUp());
        }
    }

    void navigateBack() {
        if (!backStack.isEmpty()) {
            forwardStack.push(currentPath);
            navigateToPath(backStack.pop(), false);
        }
    }

    void navigateForward() {
        if (!forwardStack.isEmpty()) {
            backStack.push(currentPath);
            navigateToPath(forwardStack.pop(), false);
        }
    }

    void navigateUp() {
        QDir currentDir(currentPath);
        if (currentDir.cdUp()) {
            navigateToPath(currentDir.absolutePath());
        }
    }

    void openFile(const QString& filePath) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }

private slots:
    void addressBarNavigateRequested(const QString& path) {
        navigateToPath(path);
    }

    void onNavigationSelection(const QItemSelection& selected,
        const QItemSelection&) {
        if (selected.indexes().isEmpty())
            return;

        QModelIndex index = selected.indexes().first();
        QString path = index.data(Qt::UserRole + 1).toString();

        if (!path.isEmpty()) {
            QFileInfo fileInfo(path);
            if (fileInfo.exists() && fileInfo.isDir()) {
                navigateToPath(path);
            }
        }
    }

    void onItemActivated(const QModelIndex& index) {
        if (!index.isValid()) return;
        
        QFileInfo fileInfo = fileModel->fileInfo(index);
        if (fileInfo.isDir()) {
            navigateToPath(fileInfo.absoluteFilePath());
        } else {
            openFile(fileInfo.absoluteFilePath());
        }
    }
};

#include "main.moc"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setStyle("Fusion");

    Explosion explorer;
    explorer.show();
    return app.exec();
}