#include "mainmenu.hpp"

#include "../networking/server.hpp"
#include "about.hpp"
#include "preferences.hpp"
#include "ui_mainmenu.h"

#include <QMessageBox>
#include <QPointer>
#include <QPushButton>
#include <QStackedWidget>

MainMenu::MainMenu(QStackedWidget *parent) : QWidget(parent), ui(new Ui::MainMenu), stack(parent)
{
	ui->setupUi(this);
	connect(ui->settingsButton, &QPushButton::pressed, this, &MainMenu::launch_preferences);
	connect(ui->startButton, &QPushButton::pressed, this, &MainMenu::launch_server);
	connect(ui->pushButton_About, &QPushButton::clicked, this, &MainMenu::launch_about);
}

MainMenu::~MainMenu()
{
	delete ui;
}

void MainMenu::launch_server()
{
	qInfo() << "Launching server from main menu";

	try
	{
		auto *server = new Server(stack);
		stack->addWidget(server);
		stack->setCurrentWidget(server);
		// When server is destroyed, remove it from the stack
		QPointer<QStackedWidget> safeStack(stack);
		connect(server, &QObject::destroyed, this, [this, server, safeStack]() {
			if (safeStack)
			{
				safeStack->removeWidget(server);
				safeStack->setCurrentWidget(this);
			}
		});
	}
	catch (const std::exception &e)
	{
		qCritical() << "Failed to launch server:" << e.what();
		QMessageBox::critical(this, tr("Error"), tr("Failed to launch server: %1").arg(e.what()));
	}
}

void MainMenu::launch_preferences()
{
	qInfo() << "Opening preferences dialog";

	auto *preferences = new Preferences();
	stack->addWidget(preferences);
	stack->setCurrentWidget(preferences);
	// When widget is destroyed, remove it from the stack
	QPointer<QStackedWidget> safeStack(stack);
	connect(preferences, &QObject::destroyed, this, [this, preferences, safeStack]() {
		if (safeStack)
		{
			safeStack->removeWidget(preferences);
			safeStack->setCurrentWidget(this);
		}
	});
}

void MainMenu::launch_about()
{
	qDebug() << "Opening about dialog";

	auto *about = new About();
	stack->addWidget(about);
	stack->setCurrentWidget(about);
	// When widget is destroyed, remove it from the stack
	QPointer<QStackedWidget> safeStack(stack);
	connect(about, &QObject::destroyed, this, [this, about, safeStack]() {
		if (safeStack)
		{
			safeStack->removeWidget(about);
			safeStack->setCurrentWidget(this);
		}
	});
}
