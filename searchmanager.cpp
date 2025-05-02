#include "searchmanager.h"
#include <QDirIterator>
#include <QTextStream>
#include <QFile>

SearchManager::SearchManager(QObject* parent)
    : QObject(parent), searchActive(false) {
    searchCriteria.caseSensitive = false;
    searchCriteria.useRegex = false;
    searchCriteria.searchContents = false;
    searchCriteria.includeSubdirectories = true;
    searchCriteria.sizeFrom = 0;
    searchCriteria.sizeTo = 0; 
}

void SearchManager::setBaseDirectory(const QString& directory) {
    baseDir = directory;
}

QString SearchManager::baseDirectory() const {
    return baseDir;
}

void SearchManager::setCriteria(const SearchCriteria& criteria) {
    searchCriteria = criteria;
}

SearchCriteria SearchManager::criteria() const {
    return searchCriteria;
}

QString SearchManager::lastQuery() const {
    return searchCriteria.text;
}

void SearchManager::quickSearch(const QString& query) {
    if (query.isEmpty()) {
        clearSearch();
        return;
    }
    
    searchCriteria.text = query;
    searchCriteria.path = baseDir;
    
    currentSearchFilters = createFilters(query);
    searchActive = true;
    
    emit searchStarted();
    emit searchFilterChanged(currentSearchFilters, true);
    
    emit searchFinished(-1);
}

void SearchManager::advancedSearch() {
    if (searchCriteria.text.isEmpty() && searchCriteria.fileTypes.isEmpty() &&
        !searchCriteria.dateFrom.isValid() && !searchCriteria.dateTo.isValid() &&
        searchCriteria.sizeFrom <= 0 && searchCriteria.sizeTo <= 0) {
        clearSearch();
        return;
    }
    
    searchActive = true;
    emit searchStarted();
    
    currentSearchFilters = createFilters(searchCriteria.text);
    emit searchFilterChanged(currentSearchFilters, true);
    emit searchFinished(-1);
}

void SearchManager::clearSearch() {
    searchActive = false;
    currentSearchFilters.clear();
    emit searchCleared();
    emit searchFilterChanged(QStringList(), false);
}

bool SearchManager::isSearchActive() const {
    return searchActive;
}

QStringList SearchManager::currentFilters() const {
    return currentSearchFilters;
}

QStringList SearchManager::createFilters(const QString& text) const {
    QStringList filters;
    
    if (text.isEmpty())
        return filters;
    
    filters << "*" + text + "*";
    
    if (!searchCriteria.fileTypes.isEmpty()) {
        for (const QString& type : searchCriteria.fileTypes) {
            filters << "*" + text + "*." + type;
        }
    }
    
    return filters;
}

bool SearchManager::matchesCriteria(const QFileInfo& fileInfo) const {
    if (!searchActive)
        return true;
    
    QString filename = fileInfo.fileName();
    
    if (!searchCriteria.text.isEmpty()) {
        if (searchCriteria.useRegex) {
            QRegularExpression regex(
                searchCriteria.text,
                searchCriteria.caseSensitive ? QRegularExpression::NoPatternOption : QRegularExpression::CaseInsensitiveOption
            );
            if (!regex.match(filename).hasMatch())
                return false;
        } else {
            Qt::CaseSensitivity cs = searchCriteria.caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
            if (!filename.contains(searchCriteria.text, cs))
                return false;
        }
    }
    
    if (fileInfo.isDir())
        return true;
    
    
    if (!searchCriteria.fileTypes.isEmpty()) {
        QString suffix = fileInfo.suffix().toLower();
        bool matchesType = false;
        for (const QString& type : searchCriteria.fileTypes) {
            if (suffix == type.toLower()) {
                matchesType = true;
                break;
            }
        }
        if (!matchesType)
            return false;
    }
    
    if (searchCriteria.sizeFrom > 0 && fileInfo.size() < searchCriteria.sizeFrom)
        return false;
    
    if (searchCriteria.sizeTo > 0 && fileInfo.size() > searchCriteria.sizeTo)
        return false;
    
    if (searchCriteria.dateFrom.isValid() && fileInfo.lastModified() < searchCriteria.dateFrom)
        return false;
    
    if (searchCriteria.dateTo.isValid() && fileInfo.lastModified() > searchCriteria.dateTo)
        return false;
    
    if (searchCriteria.searchContents && !searchCriteria.text.isEmpty()) {
        return fileContainsText(fileInfo.absoluteFilePath(), searchCriteria.text);
    }
    
    return true;
}

bool SearchManager::fileContainsText(const QString& filePath, const QString& text) const {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
        
    QTextStream in(&file);
    const int CHUNK_SIZE = 8192; 
    QString chunk;
    
    Qt::CaseSensitivity cs = searchCriteria.caseSensitive ? 
                             Qt::CaseSensitive : Qt::CaseInsensitive;
    
    if (searchCriteria.useRegex) {
        QRegularExpression regex(
            text,
            searchCriteria.caseSensitive ? 
            QRegularExpression::NoPatternOption : QRegularExpression::CaseInsensitiveOption
        );
        
        while (!(chunk = in.read(CHUNK_SIZE)).isEmpty()) {
            if (regex.match(chunk).hasMatch())
                return true;
        }
    } else {
        while (!(chunk = in.read(CHUNK_SIZE)).isEmpty()) {
            if (chunk.contains(text, cs))
                return true;
        }
    }
    
    return false;
}