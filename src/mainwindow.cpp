#include "mainwindow.hpp"
#include "networking/server.hpp"

#include "settings.hpp"
#include "ui_mainwindow.h"
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	load_settings_file(this);
	load_mouse_setting();
	load_key_maps();
	this->p = new Preferences(this);
	ui->setupUi(this);
	QPushButton::connect(ui->settingsButton, &QPushButton::pressed, this, [this] { this->p->show(); });
	QPushButton::connect(ui->startButton, &QPushButton::pressed, this, &MainWindow::launch_server);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::launch_server()
{
	Server server(this);
	server.exec();
}
