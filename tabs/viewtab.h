#ifndef VIEWTAB_H
#define VIEWTAB_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QToolButton>
#include <QAction>
#include <QActionGroup>
#include <QLabel>
#include <QFrame>
#include <QStyle>
#include "../fileviewmodel.h"

class ViewTab : public QWidget {
    Q_OBJECT
public:
    explicit ViewTab(QWidget *parent = nullptr);

signals:
    void viewModeChanged(ViewMode mode);

private:
    QActionGroup *viewModeGroup;
    
    QToolButton* createViewModeButton(QAction* action);
};

#endif