#include "mainwindow.hpp"
#include "networking/server.hpp"

#include "ui_mainwindow.h"
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	this->p = new Preferences(this);
	ui->setupUi(this);
	ui->pushButton->connect(ui->pushButton, &QPushButton::pressed, this, [=]{
		this->p->show();
	});

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
