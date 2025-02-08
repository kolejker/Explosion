#ifndef RIBBONBAR_H
#define RIBBONBAR_H

#include <QTabWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

class RibbonBar : public QWidget {
    Q_OBJECT
public:
    explicit RibbonBar(QWidget *parent = nullptr);
    
private:
    QWidget *createTab(const QString &name);
};

#endif 
