#include "mainwindow.hpp"
#include "networking/server.hpp"

#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_startButton_clicked()
{
	Server server(this);
	server.exec();
}
