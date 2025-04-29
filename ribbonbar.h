#ifndef RIBBONBAR_H
#define RIBBONBAR_H

#include <QTabWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QLineEdit>

class RibbonBar : public QWidget {
    Q_OBJECT
public:
    explicit RibbonBar(QWidget *parent = nullptr);
    QLineEdit* getAddressBar() const { return addressBar; }
    
private:
    QWidget *createFileTab();
    QWidget *createComputerTab();
    QWidget *createViewTab();
    QLineEdit* addressBar;

signals:
    void addressBarNavigated(const QString& path);

private slots:
    void onAddressBarEntered();
};

#endif