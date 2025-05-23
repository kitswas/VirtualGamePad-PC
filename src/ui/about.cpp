#include "about.hpp"

#include "ui_about.h"

About::About(QStackedWidget *parent) : QWidget(parent), ui(new Ui::About)
{
	ui->setupUi(this);
	ui->versionLabel->setText(tr("Version: %1").arg(QApplication::applicationVersion()));

	QString aboutText = tr(
		"This is a part of the [Virtual Gamepad "
		"project](https://kitswas.github.io/VirtualGamePad/)\n"
		"[![GitHub License](https://img.shields.io/github/license/kitswas/VirtualGamePad-PC) "
		"![GitHub "
		"Stars](https://img.shields.io/github/stars/kitswas/VirtualGamePad?style=social)](https://"
		"github.com/kitswas/VirtualGamePad/)\n"
		"The source code is available on [GitHub](https://github.com/Kitswas/VirtualGamePad-PC)");
	ui->descriptionBox->setMarkdown(aboutText);
	ui->descriptionBox->setReadOnly(true);
}

About::~About()
{
	delete ui;
}

void About::on_backButton_clicked()
{
	this->deleteLater();
}
