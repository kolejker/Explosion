#include "hometab.h"

HomeTab::HomeTab(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);

    QToolBar *toolbar = new QToolBar("Home");
    toolbar->addAction("Home");

    layout->addWidget(toolbar);
    setLayout(layout);
}