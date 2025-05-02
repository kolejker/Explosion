#ifndef SEARCHMANAGER_H
#define SEARCHMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QMap>
#include <QSet>
#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>

struct SearchCriteria {
    QString text;
    QString path;
    QDateTime dateFrom;
    QDateTime dateTo;
    qint64 sizeFrom;
    qint64 sizeTo;
    QStringList fileTypes;
    bool caseSensitive;
    bool useRegex;
    bool searchContents;
    bool includeSubdirectories;
};

class SearchManager : public QObject {
    Q_OBJECT
public:
    explicit SearchManager(QObject* parent = nullptr);
    
    void setBaseDirectory(const QString& directory);
    
    QString baseDirectory() const;
    
    void setCriteria(const SearchCriteria& criteria);
    
    SearchCriteria criteria() const;
    
    QString lastQuery() const;
    
    void quickSearch(const QString& query);
    
    void advancedSearch();
    
    void clearSearch();
    
    bool isSearchActive() const;
    
    QStringList currentFilters() const;
    
    bool matchesCriteria(const QFileInfo& fileInfo) const;

signals:
    void searchFilterChanged(const QStringList& filters, bool hideNonMatching);
    
    void searchStarted();
    
    void searchFinished(int resultCount);
    
    void searchCleared();

private:
    QString baseDir;
    SearchCriteria searchCriteria;
    bool searchActive;
    QStringList currentSearchFilters;
    
    QStringList createFilters(const QString& text) const;
    
    bool fileMatchesAdvancedCriteria(const QFileInfo& fileInfo) const;
    
    bool fileContainsText(const QString& filePath, const QString& text) const;
};

#endif 