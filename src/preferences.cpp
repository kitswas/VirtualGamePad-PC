#include "preferences.h"
#include "settings_key_variables.h"
#include "settings.h"
#include "ui_preferences.h"
#include <QDebug>
#include <QSlider>

Preferences::Preferences(QWidget *parent) : QDialog(parent), ui(new Ui::Preferences)
{
	ui->setupUi(this);
	ui->horizontalSlider->adjustSize();
	this->adjustSize();
	ui->buttonBox->connect(ui->buttonBox, &QDialogButtonBox::accepted, [=] {
		this->change_mouse_sensitivity(ui->horizontalSlider->value() * 100);
		qDebug() << mouse_sensivity << Qt::endl;
		save_setting(setting_keys::Mouse_sensivity, mouse_sensivity / 100);
	});
	ui->horizontalSlider->setValue(mouse_sensivity / 100);
}

int Preferences::change_mouse_sensitivity(int value)
{
	mouse_sensivity = value;
	return 1;
}

Preferences::~Preferences()
{
	delete ui;
}
