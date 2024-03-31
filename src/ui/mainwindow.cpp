#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "../settings.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	load_settings_file(this);
	load_mouse_setting();
	load_key_maps();
	ui->setupUi(this);
	ui->stackedWidget->addWidget(new MainMenu(ui->stackedWidget));
}

MainWindow::~MainWindow()
{
	delete ui;
}
