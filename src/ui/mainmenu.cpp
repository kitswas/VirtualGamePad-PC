#include "mainmenu.hpp"

#include "../networking/server.hpp"
#include "preferences.hpp"
#include "ui_mainmenu.h"

#include <QPushButton>
#include <QStackedWidget>

MainMenu::MainMenu(QStackedWidget *parent) : QWidget(parent), ui(new Ui::MainMenu), stack(parent)
{
	ui->setupUi(this);
	connect(ui->settingsButton, &QPushButton::pressed, this, &MainMenu::launch_preferences);
	connect(ui->startButton, &QPushButton::pressed, this, &MainMenu::launch_server);
}

MainMenu::~MainMenu()
{
	delete ui;
}

void MainMenu::launch_server()
{
	auto *server = new Server(stack);
	stack->addWidget(server);
	stack->setCurrentWidget(server);
	// When server is destroyed, remove it from the stack
	connect(server, &QObject::destroyed, [this, server]() {
		stack->removeWidget(server);
		stack->setCurrentWidget(this);
	});
}

void MainMenu::launch_preferences()
{
	auto *preferences = new Preferences();
	stack->addWidget(preferences);
	stack->setCurrentWidget(preferences);
	// When widget is destroyed, remove it from the stack
	connect(preferences, &QObject::destroyed, [this, preferences]() {
		stack->removeWidget(preferences);
		stack->setCurrentWidget(this);
	});
}
