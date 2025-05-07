#include "sharetab.h"

ShareTab::ShareTab(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);

    QToolBar *toolbar = new QToolBar("Share");
    toolbar->addAction("Share");

    layout->addWidget(toolbar);
    setLayout(layout);
}