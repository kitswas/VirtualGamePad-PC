#include "preferences.h"
#include "ui_preferences.h"
#include "networking/executor.hpp"
#include <QSlider>
#include <QDebug>

Preferences::Preferences(QWidget *parent) :
	  QDialog(parent),
	  ui(new Ui::Preferences)
{
	ui->setupUi(this);
	ui->horizontalSlider->adjustSize();
	this->adjustSize();
	ui->buttonBox->connect(ui->buttonBox, &QDialogButtonBox::accepted, [=]{
		this->change_mouse_sensitivity(ui->horizontalSlider->value()*100);
		qDebug() << mouse_sensivity << Qt::endl;
	});

}

int Preferences::change_mouse_sensitivity(int value) {
	mouse_sensivity = value;
	return 1;
}


Preferences::~Preferences()
{
	delete ui;
}
