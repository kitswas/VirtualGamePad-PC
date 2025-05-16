#include "mainwindow.hpp"

#include "../settings/settings_singleton.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	SettingsSingleton::instance().loadAll();
	ui->setupUi(this);
	ui->stackedWidget->addWidget(new MainMenu(ui->stackedWidget));
}

MainWindow::~MainWindow()
{
	delete ui;
}
