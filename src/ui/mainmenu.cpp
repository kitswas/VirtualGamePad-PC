#include "mainmenu.hpp"
#include "ui_mainmenu.h"

#include "../networking/server.hpp"
#include "preferences.hpp"

#include <QMdiArea>
#include <QPushButton>
#include <QStackedWidget>

MainMenu::MainMenu(QStackedWidget *parent) : QWidget(parent), ui(new Ui::MainMenu)
{
	ui->setupUi(this);
	stack = parent;
	QPushButton::connect(ui->settingsButton, &QPushButton::pressed, this,
						 &MainMenu::launch_preferences);
	QPushButton::connect(ui->startButton, &QPushButton::pressed, this, &MainMenu::launch_server);
}

MainMenu::~MainMenu()
{
	delete ui;
}

void MainMenu::launch_server()
{
	QWidget *server = new Server(stack);
	stack->addWidget(server);
	stack->setCurrentWidget(server);
	// When server is destroyed, remove it from the stack
	QObject::connect(server, &QObject::destroyed, [this, server]() {
		stack->removeWidget(server);
		stack->setCurrentWidget(this);
	});
}

//! TODO: Test this rigorously
void MainMenu::launch_preferences()
{
	QWidget *preferences = new Preferences();
	stack->addWidget(preferences);
	stack->setCurrentWidget(preferences);
	// When widget is destroyed, remove it from the stack
	QWidget::connect(preferences, &QObject::destroyed, [this, preferences]() {
		stack->removeWidget(preferences);
		stack->setCurrentWidget(this);
	});
}
