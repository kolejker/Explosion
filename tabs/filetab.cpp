#include "filetab.h"

FileTab::FileTab(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);

    QToolBar *toolbar = new QToolBar("File");
    toolbar->addAction("File");

    layout->addWidget(toolbar);
    setLayout(layout);
}