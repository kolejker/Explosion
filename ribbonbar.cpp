#include "ribbonbar.h"
#include <QStyle>
#include <QPushButton>
#include <QLabel>

RibbonBar::RibbonBar(QWidget *parent): QWidget(parent) {
    setFixedHeight(180);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QTabWidget *tabWidget = new QTabWidget(this);
    tabWidget->setTabPosition(QTabWidget::North);
    tabWidget->addTab(createFileTab(), "File");
    tabWidget->addTab(createComputerTab(), "Computer");
    tabWidget->addTab(createViewTab(), "View");

    QToolBar *toolbar = new QToolBar("Navigation");
    toolbar->setMovable(false);
    toolbar->addAction(style()->standardIcon(QStyle::SP_ArrowBack), "Back");
    toolbar->addAction(style()->standardIcon(QStyle::SP_ArrowForward), "Forward");
    toolbar->addAction(style()->standardIcon(QStyle::SP_ArrowUp), "Up");
    
    addressBar = new QLineEdit();
    addressBar->setMinimumWidth(300);
    addressBar->setClearButtonEnabled(true);
    addressBar->setPlaceholderText("Address");
    toolbar->addWidget(addressBar);
    
    connect(addressBar, &QLineEdit::returnPressed, this, &RibbonBar::onAddressBarEntered);

    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(toolbar);
    setLayout(mainLayout);
}

void RibbonBar::onAddressBarEntered() {
    QString path = addressBar->text();
    emit addressBarNavigated(path);
}

QWidget *RibbonBar::createFileTab() {
    QWidget *tab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(tab);
    
    QToolBar *toolbar = new QToolBar("File");
    toolbar->addAction("File");
    
    layout->addWidget(toolbar);
    tab->setLayout(layout);
    return tab;
}

QWidget *RibbonBar::createComputerTab() {
    QWidget *tab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(tab);
    
    QToolBar *toolbar = new QToolBar("Computer");
    toolbar->addAction("Computer");
    
    layout->addWidget(toolbar);
    tab->setLayout(layout);
    return tab;
}

QWidget *RibbonBar::createViewTab() {
    QWidget *tab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(tab);
    
    QToolBar *toolbar = new QToolBar("View");
    toolbar->addAction("View");

    layout->addWidget(toolbar);
    tab->setLayout(layout);
    return tab;
}