#include "ribbonbar.h"

#include <QStyle>

RibbonBar::RibbonBar(QWidget * parent): QWidget(parent) {
  setFixedHeight(180);
  QVBoxLayout * mainLayout = new QVBoxLayout(this);
  mainLayout -> setContentsMargins(0, 0, 0, 0);
  mainLayout -> setSpacing(0);

  QTabWidget * tabWidget = new QTabWidget(this);
  tabWidget -> setTabPosition(QTabWidget::North);
  tabWidget -> addTab(createTab("File"), "File");
  tabWidget -> addTab(createTab("Computer"), "Computer");
  tabWidget -> addTab(createTab("View"), "View");

  QToolBar * toolbar = new QToolBar("Navigation");
  toolbar -> setMovable(false);

  toolbar -> addAction(style() -> standardIcon(QStyle::SP_ArrowBack), "Back");
  toolbar -> addAction(style() -> standardIcon(QStyle::SP_ArrowForward), "Forward");
  toolbar -> addAction(style() -> standardIcon(QStyle::SP_ArrowUp), "Up");

  mainLayout -> addWidget(tabWidget);
  mainLayout -> addWidget(toolbar);

  setLayout(mainLayout);
}

QWidget * RibbonBar::createTab(const QString & name) {
  QWidget * tab = new QWidget;
  QVBoxLayout * layout = new QVBoxLayout(tab);

  QToolBar * toolbar = new QToolBar(name);
  toolbar -> addAction("Action 1");
  toolbar -> addAction("Action 2");
  toolbar -> addAction("Action 3");

  layout -> addWidget(toolbar);
  tab -> setLayout(layout);
  return tab;
}