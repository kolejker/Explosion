#include <QApplication>
#include <QMainWindow>
#include <QTreeView>
#include <QListView>
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
#include <QStackedWidget>

#include "ribbonbar.h"
#include "fileviewmodel.h"
#include "searchmanager.h"
#include "tabs/filetab.h"
#include "tabs/hometab.h"
#include "tabs/sharetab.h"
#include "tabs/viewtab.h"

class Explosion: public QMainWindow {
    Q_OBJECT
public:
    Explosion(QWidget* parent = nullptr): QMainWindow(parent) {
        setWindowTitle("File Explorer");
        setupUI();
        setupQuickAccess();
        
        fileViewModel = new FileViewModel(this);
        fileViewModel->setupFileSystem(viewContainer);
        
        searchManager = new SearchManager(this);
        
        connect(searchManager, &SearchManager::searchFilterChanged, 
                this, &Explosion::onSearchFilterChanged);
        connect(searchManager, &SearchManager::searchStarted,
                this, &Explosion::onSearchStarted);
        connect(searchManager, &SearchManager::searchFinished,
                this, &Explosion::onSearchFinished);
        connect(searchManager, &SearchManager::searchCleared,
                this, &Explosion::onSearchCleared);
        
        connect(fileViewModel, &FileViewModel::itemActivated, 
                this, &Explosion::onItemActivated);
        
        connect(ribbon, &RibbonBar::recentFolderNavigated, this, &Explosion::onRecentFolderSelected);
        
        connect(ribbon, &RibbonBar::addressBarNavigated, this, &Explosion::addressBarNavigateRequested);
        connect(ribbon, &RibbonBar::searchRequested, this, &Explosion::performSearch);
        connect(ribbon, &RibbonBar::viewModeChanged, this, &Explosion::onViewModeChanged);
        
        currentPath = QDir::homePath();
        navigateToPath(currentPath);
    }

private:
    QTreeView* navigationTree;
    QStackedWidget* viewContainer;  
    QStandardItemModel* quickAccessModel;
    QTreeView* quickAccessTree;
    QStack<QString> backStack;
    QStack<QString> forwardStack;
    QString currentPath;
    RibbonBar* ribbon;
    QLineEdit* addressBar;
    
    FileViewModel* fileViewModel;
    SearchManager* searchManager;
    
    void setupUI() {
        QWidget* centralWidget = new QWidget(this);
        QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setContentsMargins(0, 0, 0, 0);

        ribbon = new RibbonBar(this);
        addressBar = ribbon->getAddressBar();
        QLineEdit* searchBar = ribbon->getSearchBar();
        connect(ribbon, &RibbonBar::addressBarNavigated, this, &Explosion::addressBarNavigateRequested);
        connect(ribbon, &RibbonBar::searchRequested, this, &Explosion::performSearch);

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

        viewContainer = new ResizableStackedWidget();

        splitter->addWidget(leftPanel);
        splitter->addWidget(viewContainer);
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
            QAction* upAction = navToolBar->actions()[3]; 

            connect(backAction, &QAction::triggered, this, &Explosion::navigateBack);
            connect(forwardAction, &QAction::triggered, this, &Explosion::navigateForward);
            connect(upAction, &QAction::triggered, this, &Explosion::navigateUp);

            backAction->setEnabled(false);
            forwardAction->setEnabled(false);
        }

        connect(static_cast<ResizableStackedWidget*>(viewContainer), &ResizableStackedWidget::resized,
        this, [this]() {
            if (fileViewModel)
                fileViewModel->onContainerResized();
        });

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

    void updateAddressBar(const QString& path) {
        if (addressBar) {
            if (searchManager->isSearchActive()) {
                addressBar->setText(QString("Search results for \"%1\" in %2").arg(searchManager->lastQuery()).arg(path));
            } else {
                addressBar->setText(path);
            }
        }
    }

    

    void navigateToPath(const QString& path, bool addToHistory = true) {
        if (!QFileInfo::exists(path)) return;
        
        if (addToHistory && !currentPath.isEmpty()) {
            backStack.push(currentPath);
            forwardStack.clear();
        }
        
        currentPath = path;
        fileViewModel->setRootPath(path);
        searchManager->setBaseDirectory(path);
        
        if (searchManager->isSearchActive()) {
            searchManager->clearSearch();
        }
        
        ribbon->updateRecentFolders(path);
        
        statusBar()->showMessage("Location: " + path);
        updateAddressBar(path);
        
        QToolBar* navToolBar = findChild<QToolBar*>();
        if (navToolBar) {
            navToolBar->actions()[0]->setEnabled(!backStack.isEmpty());
            navToolBar->actions()[1]->setEnabled(!forwardStack.isEmpty());
            navToolBar->actions()[3]->setEnabled(QDir(path).cdUp());
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
    
    void onRecentFolderSelected(const QString& path) {
        navigateToPath(path);
    }

    void performSearch(const QString& searchText) {
        searchManager->quickSearch(searchText);
    }
    
    void onSearchFilterChanged(const QStringList& filters, bool hideNonMatching) {
        fileViewModel->applySearchFilter(filters, hideNonMatching);
        updateAddressBar(currentPath);
    }
    
    void onSearchStarted() {
        statusBar()->showMessage("Searching...");
    }
    
    void onSearchFinished(int resultCount) {
        if (resultCount >= 0) {
            statusBar()->showMessage(QString("Found %1 results for \"%2\"")
                .arg(resultCount)
                .arg(searchManager->lastQuery()));
        } else {
            statusBar()->showMessage(QString("Searching for \"%1\" in %2")
                .arg(searchManager->lastQuery())
                .arg(currentPath));
        }
    }
    
    void onSearchCleared() {
        statusBar()->showMessage("Search cleared");
    }
    
    void onNavigationSelection(const QItemSelection& selected, const QItemSelection&) {
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

        QFileInfo fileInfo = fileViewModel->model()->fileInfo(index);
        if (fileInfo.isDir()) {
            navigateToPath(fileInfo.absoluteFilePath());
        } else {
            openFile(fileInfo.absoluteFilePath());
        }
    }

    void onViewModeChanged(ViewMode mode) {
        if (fileViewModel) {
            fileViewModel->setViewMode(mode);
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