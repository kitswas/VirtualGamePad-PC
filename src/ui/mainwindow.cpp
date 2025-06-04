#include "mainwindow.hpp"

#include "../settings/settings_singleton.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	qDebug() << "Initializing MainWindow";

	SettingsSingleton::instance().loadAll();
	ui->setupUi(this);
	ui->stackedWidget->addWidget(new MainMenu(ui->stackedWidget));

	qDebug() << "MainWindow initialized successfully";
}

MainWindow::~MainWindow()
{
	qDebug() << "Destroying MainWindow";
	delete ui;
}
