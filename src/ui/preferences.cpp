#include "preferences.hpp"
#include "../settings/input_types.hpp"
#include "../settings/settings.hpp"
#include "../settings/settings_key_variables.hpp"
#include "../settings/settings_singleton.hpp"
#include "ui_preferences.h"
#include "winuser.h"
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QKeyEvent>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QProcess>
#include <QSlider>
#include <QStandardPaths>

Preferences::Preferences(QWidget *parent) : QWidget(parent), ui(new Ui::Preferences)
{
	ui->setupUi(this);
	setupKeymapTabs();
	install_event_filter();
	ui->horizontalSlider->adjustSize();
	ui->buttonBox->connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [this] {
		SettingsSingleton::instance().setMouseSensitivity(ui->horizontalSlider->value() * 100);
		qDebug() << SettingsSingleton::instance().mouseSensitivity();
		change_key_inputs();
	});
	ui->buttonBox->connect(ui->buttonBox, &QDialogButtonBox::rejected, this, [this] {
		load_keys();
		this->deleteLater();
	});
	ui->buttonBox->connect(ui->buttonBox, &QDialogButtonBox::accepted, this,
						   [this] { this->deleteLater(); });
	ui->buttonBox->connect(ui->buttonBox, &QDialogButtonBox::helpRequested, this,
						   &Preferences::show_help);
	ui->buttonBox->setCenterButtons(true);
	ui->horizontalSlider->setValue(SettingsSingleton::instance().mouseSensitivity() / 100);

	setup_profile_management();
	load_keys();
	load_thumbsticks();
}

Preferences::~Preferences()
{
	delete ui;
}

void Preferences::setupKeymapTabs()
{
	// This function can be used to connect signals/slots for the new tab structure if needed.
}

void Preferences::setup_profile_management()
{
	// Make sure profiles directory exists
	QDir dir(get_profiles_dir());
	if (!dir.exists())
	{
		dir.mkpath(".");
	}

	// Set up signals and slots
	connect(ui->loadProfileButton, &QPushButton::clicked, this, &Preferences::load_profile);
	connect(ui->saveProfileButton, &QPushButton::clicked, this, &Preferences::save_profile);
	connect(ui->newProfileButton, &QPushButton::clicked, this, &Preferences::new_profile);
	connect(ui->deleteProfileButton, &QPushButton::clicked, this, &Preferences::delete_profile);
	connect(ui->profileComboBox, &QComboBox::currentTextChanged, this,
			&Preferences::profile_selection_changed);

	// Initialize with available profiles
	refresh_profile_list();

	// Set default profile as "Default" if it exists
	int defaultIndex = ui->profileComboBox->findText("Default");
	if (defaultIndex >= 0)
	{
		ui->profileComboBox->setCurrentIndex(defaultIndex);
	}
}

QString Preferences::get_profiles_dir() const
{
	return QDir::toNativeSeparators(qApp->applicationDirPath() + "/profiles");
}

void Preferences::refresh_profile_list()
{
	ui->profileComboBox->blockSignals(true);
	ui->profileComboBox->clear();

	QDir dir(get_profiles_dir());
	QStringList nameFilters;
	nameFilters << "*.ini";

	QFileInfoList fileList = dir.entryInfoList(nameFilters, QDir::Files, QDir::Name);

	foreach (const QFileInfo &fileInfo, fileList)
	{
		QString profileName = fileInfo.baseName();
		ui->profileComboBox->addItem(profileName);
	}

	// If no profiles exist, add a default one
	if (ui->profileComboBox->count() == 0)
	{
		ui->profileComboBox->addItem("Default");
	}

	ui->profileComboBox->blockSignals(false);
}

void Preferences::load_profile()
{
	QString profileName = ui->profileComboBox->currentText();
	if (profileName.isEmpty())
	{
		return;
	}

	QString profilePath = get_profiles_dir() + "/" + profileName + ".ini";
	if (load_profile_from_file(profilePath))
	{
		QMessageBox::information(this, "Profile Loaded",
								 "Profile '" + profileName + "' loaded successfully");
	}
	else
	{
		QMessageBox::warning(this, "Error", "Failed to load profile '" + profileName + "'");
	}
}

bool Preferences::load_profile_from_file(const QString &profilePath)
{
	QSettings profileSettings(profilePath, QSettings::IniFormat);

	// Read key mappings
	for (auto it = keymaps.begin(); it != keymaps.end(); ++it)
	{
		QString settingKey = it.value();
		int vk = profileSettings.value(settingKey, -1).toInt();

		if (vk != -1)
		{
			// Update temp map to hold the new value
			QString objName;

			// Map setting key to UI element name
			if (settingKey == keymaps[setting_keys::keys::X])
				objName = "xmap";
			else if (settingKey == keymaps[setting_keys::keys::Y])
				objName = "ymap";
			else if (settingKey == keymaps[setting_keys::keys::A])
				objName = "amap";
			else if (settingKey == keymaps[setting_keys::keys::B])
				objName = "bmap";
			else if (settingKey == keymaps[setting_keys::keys::LSHDR])
				objName = "Ltmap";
			else if (settingKey == keymaps[setting_keys::keys::RSHDR])
				objName = "Rtmap";
			else if (settingKey == keymaps[setting_keys::keys::DPADDOWN])
				objName = "ddownmap";
			else if (settingKey == keymaps[setting_keys::keys::DPADUP])
				objName = "dupmap";
			else if (settingKey == keymaps[setting_keys::keys::DPADLEFT])
				objName = "dleftmap";
			else if (settingKey == keymaps[setting_keys::keys::DPADRIGHT])
				objName = "drightmap";
			else if (settingKey == keymaps[setting_keys::keys::VIEW])
				objName = "viewmap";
			else if (settingKey == keymaps[setting_keys::keys::MENU])
				objName = "menumap";

			if (!objName.isEmpty())
			{
				temp[objName] = vk;

				// Update display text
				char buffer[256];
				get_scan_code(vk, buffer, 256);

				QLineEdit *lineEdit = findChild<QLineEdit *>(objName);
				if (lineEdit)
				{
					lineEdit->setText(QString(buffer));
				}
			}
		}
	}

	// Read thumbstick mappings
	bool leftThumbMouseMove =
		profileSettings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstick], false)
			.toBool();
	ui->leftThumbMouseMove->setChecked(leftThumbMouseMove);

	ui->leftThumbUpMap->setText(QString::number(
		profileSettings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickUpKey], 'W')
			.toInt()));

	ui->leftThumbDownMap->setText(QString::number(
		profileSettings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickDownKey], 'S')
			.toInt()));

	ui->leftThumbLeftMap->setText(QString::number(
		profileSettings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickLeftKey], 'A')
			.toInt()));

	ui->leftThumbRightMap->setText(QString::number(
		profileSettings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickRightKey], 'D')
			.toInt()));

	bool rightThumbMouseMove =
		profileSettings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstick], true)
			.toBool();
	ui->rightThumbMouseMove->setChecked(rightThumbMouseMove);

	ui->rightThumbUpMap->setText(QString::number(
		profileSettings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickUpKey], VK_UP)
			.toInt()));

	ui->rightThumbDownMap->setText(QString::number(
		profileSettings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickDownKey],
				   VK_DOWN)
			.toInt()));

	ui->rightThumbLeftMap->setText(QString::number(
		profileSettings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickLeftKey],
				   VK_LEFT)
			.toInt()));

	ui->rightThumbRightMap->setText(QString::number(
		profileSettings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickRightKey],
				   VK_RIGHT)
			.toInt()));

	return true;
}

void Preferences::save_profile()
{
	QString profileName = ui->profileComboBox->currentText();
	if (profileName.isEmpty())
	{
		new_profile();
		return;
	}

	QString profilePath = get_profiles_dir() + "/" + profileName + ".ini";
	save_profile_to_file(profilePath);
	QMessageBox::information(this, "Profile Saved",
							 "Profile '" + profileName + "' saved successfully to " + profilePath);
}

void Preferences::save_profile_to_file(const QString &profilePath)
{
	QSettings profileSettings(profilePath, QSettings::IniFormat);

	// Save button mappings
	profileSettings.setValue(keymaps[setting_keys::keys::X], temp[ui->xmap->objectName()]);
	profileSettings.setValue(keymaps[setting_keys::keys::Y], temp[ui->ymap->objectName()]);
	profileSettings.setValue(keymaps[setting_keys::keys::A], temp[ui->amap->objectName()]);
	profileSettings.setValue(keymaps[setting_keys::keys::B], temp[ui->bmap->objectName()]);
	profileSettings.setValue(keymaps[setting_keys::keys::LSHDR], temp[ui->Ltmap->objectName()]);
	profileSettings.setValue(keymaps[setting_keys::keys::RSHDR], temp[ui->Rtmap->objectName()]);
	profileSettings.setValue(keymaps[setting_keys::keys::DPADDOWN],
							 temp[ui->ddownmap->objectName()]);
	profileSettings.setValue(keymaps[setting_keys::keys::DPADUP], temp[ui->dupmap->objectName()]);
	profileSettings.setValue(keymaps[setting_keys::keys::DPADLEFT],
							 temp[ui->dleftmap->objectName()]);
	profileSettings.setValue(keymaps[setting_keys::keys::DPADRIGHT],
							 temp[ui->drightmap->objectName()]);
	profileSettings.setValue(keymaps[setting_keys::keys::VIEW], temp[ui->viewmap->objectName()]);
	profileSettings.setValue(keymaps[setting_keys::keys::MENU], temp[ui->menumap->objectName()]);

	// Save thumbstick mappings
	profileSettings.setValue(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstick],
							 ui->leftThumbMouseMove->isChecked());
	profileSettings.setValue(
		thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickUpKey],
		ui->leftThumbUpMap->text().toInt());
	profileSettings.setValue(
		thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickDownKey],
		ui->leftThumbDownMap->text().toInt());
	profileSettings.setValue(
		thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickLeftKey],
		ui->leftThumbLeftMap->text().toInt());
	profileSettings.setValue(
		thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickRightKey],
		ui->leftThumbRightMap->text().toInt());

	profileSettings.setValue(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstick],
							 ui->rightThumbMouseMove->isChecked());
	profileSettings.setValue(
		thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickUpKey],
		ui->rightThumbUpMap->text().toInt());
	profileSettings.setValue(
		thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickDownKey],
		ui->rightThumbDownMap->text().toInt());
	profileSettings.setValue(
		thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickLeftKey],
		ui->rightThumbLeftMap->text().toInt());
	profileSettings.setValue(
		thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickRightKey],
		ui->rightThumbRightMap->text().toInt());

	profileSettings.sync();
}

void Preferences::new_profile()
{
	bool ok;
	QString profileName = QInputDialog::getText(this, "New Profile",
												"Enter profile name:", QLineEdit::Normal, "", &ok);
	if (ok && !profileName.isEmpty())
	{
		// Check if profile already exists
		QDir dir(get_profiles_dir());
		if (QFile::exists(dir.filePath(profileName + ".ini")))
		{
			QMessageBox::warning(
				this, "Profile Exists",
				"A profile with this name already exists. Please choose a different name.");
			return;
		}

		// Add to combo box
		ui->profileComboBox->addItem(profileName);
		ui->profileComboBox->setCurrentText(profileName);

		// Save current settings to this profile
		QString profilePath = get_profiles_dir() + "/" + profileName + ".ini";
		save_profile_to_file(profilePath);

		QMessageBox::information(this, "Profile Created",
								 "Profile '" + profileName + "' created successfully");
	}
}

void Preferences::delete_profile()
{
	QString profileName = ui->profileComboBox->currentText();
	if (profileName.isEmpty())
	{
		return;
	}

	// Confirm deletion
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, "Delete Profile",
								  "Are you sure you want to delete profile '" + profileName + "'?",
								  QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::Yes)
	{
		QString profilePath = get_profiles_dir() + "/" + profileName + ".ini";
		QFile file(profilePath);

		if (file.remove())
		{
			refresh_profile_list();
			QMessageBox::information(this, "Profile Deleted",
									 "Profile '" + profileName + "' deleted successfully");
		}
		else
		{
			QMessageBox::warning(this, "Error", "Failed to delete profile '" + profileName + "'");
		}
	}
}

void Preferences::profile_selection_changed(const QString &profileName)
{
	if (profileName.isEmpty())
	{
		return;
	}

	// Store the current profile name
	currentProfile = profileName;
}

void Preferences::load_thumbsticks()
{
	auto &settings_singleton = SettingsSingleton::instance();
	auto &thumbsticks = settings_singleton.thumbstickInputs();

	// Left thumbstick
	ui->leftThumbMouseMove->setChecked(thumbsticks[Thumbstick_Left].is_mouse_move);

	ui->leftThumbUpMap->setText(QString::number(thumbsticks[Thumbstick_Left].up.vk));
	ui->leftThumbDownMap->setText(QString::number(thumbsticks[Thumbstick_Left].down.vk));
	ui->leftThumbLeftMap->setText(QString::number(thumbsticks[Thumbstick_Left].left.vk));
	ui->leftThumbRightMap->setText(QString::number(thumbsticks[Thumbstick_Left].right.vk));

	// Right thumbstick
	ui->rightThumbMouseMove->setChecked(thumbsticks[Thumbstick_Right].is_mouse_move);

	ui->rightThumbUpMap->setText(QString::number(thumbsticks[Thumbstick_Right].up.vk));
	ui->rightThumbDownMap->setText(QString::number(thumbsticks[Thumbstick_Right].down.vk));
	ui->rightThumbLeftMap->setText(QString::number(thumbsticks[Thumbstick_Right].left.vk));
	ui->rightThumbRightMap->setText(QString::number(thumbsticks[Thumbstick_Right].right.vk));
}

void Preferences::change_key_inputs()
{
	auto &buttons = SettingsSingleton::instance().gamepadButtons();
	buttons[GamepadButtons::GamepadButtons_X].vk = this->temp[ui->xmap->objectName()];
	buttons[GamepadButtons::GamepadButtons_X].is_mouse_button =
		is_mouse_button(this->temp[ui->xmap->objectName()]);
	SettingsSingleton::instance().saveSetting(keymaps[setting_keys::keys::X],
											  this->temp[ui->xmap->objectName()]);
	/*------------------------------------------------------------*/
	buttons[GamepadButtons::GamepadButtons_Y].vk = this->temp[ui->ymap->objectName()];
	buttons[GamepadButtons::GamepadButtons_Y].is_mouse_button =
		is_mouse_button(this->temp[ui->ymap->objectName()]);
	SettingsSingleton::instance().saveSetting(keymaps[setting_keys::keys::Y],
											  this->temp[ui->ymap->objectName()]);
	/*------------------------------------------------------------*/
	buttons[GamepadButtons::GamepadButtons_A].vk = this->temp[ui->amap->objectName()];
	buttons[GamepadButtons::GamepadButtons_A].is_mouse_button =
		is_mouse_button(this->temp[ui->amap->objectName()]);
	SettingsSingleton::instance().saveSetting(keymaps[setting_keys::keys::A],
											  this->temp[ui->amap->objectName()]);
	/*------------------------------------------------------------*/
	buttons[GamepadButtons::GamepadButtons_B].vk = this->temp[ui->bmap->objectName()];
	buttons[GamepadButtons::GamepadButtons_B].is_mouse_button =
		is_mouse_button(this->temp[ui->bmap->objectName()]);
	SettingsSingleton::instance().saveSetting(keymaps[setting_keys::keys::B],
											  this->temp[ui->bmap->objectName()]);
	/*------------------------------------------------------------*/
	buttons[GamepadButtons::GamepadButtons_LeftShoulder].vk = this->temp[ui->Ltmap->objectName()];
	buttons[GamepadButtons::GamepadButtons_LeftShoulder].is_mouse_button =
		is_mouse_button(this->temp[ui->Ltmap->objectName()]);
	SettingsSingleton::instance().saveSetting(keymaps[setting_keys::keys::LSHDR],
											  this->temp[ui->Ltmap->objectName()]);
	/*------------------------------------------------------------*/
	buttons[GamepadButtons::GamepadButtons_RightShoulder].vk = this->temp[ui->Rtmap->objectName()];
	buttons[GamepadButtons::GamepadButtons_RightShoulder].is_mouse_button =
		is_mouse_button(this->temp[ui->Rtmap->objectName()]);
	SettingsSingleton::instance().saveSetting(keymaps[setting_keys::keys::RSHDR],
											  this->temp[ui->Rtmap->objectName()]);
	/*------------------------------------------------------------*/
	buttons[GamepadButtons::GamepadButtons_DPadDown].vk = this->temp[ui->ddownmap->objectName()];
	buttons[GamepadButtons::GamepadButtons_DPadDown].is_mouse_button =
		is_mouse_button(this->temp[ui->ddownmap->objectName()]);
	SettingsSingleton::instance().saveSetting(keymaps[setting_keys::keys::DPADDOWN],
											  this->temp[ui->ddownmap->objectName()]);
	/*------------------------------------------------------------*/
	buttons[GamepadButtons::GamepadButtons_DPadUp].vk = this->temp[ui->dupmap->objectName()];
	buttons[GamepadButtons::GamepadButtons_DPadUp].is_mouse_button =
		is_mouse_button(this->temp[ui->dupmap->objectName()]);
	SettingsSingleton::instance().saveSetting(keymaps[setting_keys::keys::DPADUP],
											  this->temp[ui->dupmap->objectName()]);
	/*------------------------------------------------------------*/
	buttons[GamepadButtons::GamepadButtons_DPadRight].vk = this->temp[ui->drightmap->objectName()];
	buttons[GamepadButtons::GamepadButtons_DPadRight].is_mouse_button =
		is_mouse_button(this->temp[ui->drightmap->objectName()]);
	SettingsSingleton::instance().saveSetting(keymaps[setting_keys::keys::DPADRIGHT],
											  this->temp[ui->drightmap->objectName()]);
	/*------------------------------------------------------------*/
	buttons[GamepadButtons::GamepadButtons_DPadLeft].vk = this->temp[ui->dleftmap->objectName()];
	buttons[GamepadButtons::GamepadButtons_DPadLeft].is_mouse_button =
		is_mouse_button(this->temp[ui->dleftmap->objectName()]);
	SettingsSingleton::instance().saveSetting(keymaps[setting_keys::keys::DPADLEFT],
											  this->temp[ui->dleftmap->objectName()]);
	/*-----------------------------------------------------------*/
	buttons[GamepadButtons::GamepadButtons_View].vk = this->temp[ui->viewmap->objectName()];
	buttons[GamepadButtons::GamepadButtons_View].is_mouse_button =
		is_mouse_button(this->temp[ui->viewmap->objectName()]);
	SettingsSingleton::instance().saveSetting(keymaps[setting_keys::keys::VIEW],
											  this->temp[ui->viewmap->objectName()]);
	/*-----------------------------------------------------------*/
	buttons[GamepadButtons::GamepadButtons_Menu].vk = this->temp[ui->menumap->objectName()];
	buttons[GamepadButtons::GamepadButtons_Menu].is_mouse_button =
		is_mouse_button(this->temp[ui->menumap->objectName()]);
	SettingsSingleton::instance().saveSetting(keymaps[setting_keys::keys::MENU],
											  this->temp[ui->menumap->objectName()]);
	/*-----------------------------------------------------------*/

	// Thumbstick mappings
	change_thumbstick_inputs();
}

void Preferences::change_thumbstick_inputs()
{
	auto &settings_singleton = SettingsSingleton::instance();
	auto &thumbsticks = settings_singleton.thumbstickInputs();

	// Left thumbstick
	thumbsticks[Thumbstick_Left].is_mouse_move = ui->leftThumbMouseMove->isChecked();

	thumbsticks[Thumbstick_Left].up.vk = ui->leftThumbUpMap->text().toUShort();
	thumbsticks[Thumbstick_Left].down.vk = ui->leftThumbDownMap->text().toUShort();
	thumbsticks[Thumbstick_Left].left.vk = ui->leftThumbLeftMap->text().toUShort();
	thumbsticks[Thumbstick_Left].right.vk = ui->leftThumbRightMap->text().toUShort();

	settings_singleton.setThumbstickInput(Thumbstick_Left, thumbsticks[Thumbstick_Left]);

	// Right thumbstick
	thumbsticks[Thumbstick_Right].is_mouse_move = ui->rightThumbMouseMove->isChecked();

	thumbsticks[Thumbstick_Right].up.vk = ui->rightThumbUpMap->text().toUShort();
	thumbsticks[Thumbstick_Right].down.vk = ui->rightThumbDownMap->text().toUShort();
	thumbsticks[Thumbstick_Right].left.vk = ui->rightThumbLeftMap->text().toUShort();
	thumbsticks[Thumbstick_Right].right.vk = ui->rightThumbRightMap->text().toUShort();

	settings_singleton.setThumbstickInput(Thumbstick_Right, thumbsticks[Thumbstick_Right]);
}

/**
 * Copies the name of the of corresponding key or virtual key code to the provided buffer.
 * @param vk
 * The virtual key code of the key you want to get.
 * @param a
 * The buffer to store the name of the key.
 * @param size
 * Size of the buffer(in char) in which the name is stored to ensure memory safety
 */
void Preferences::get_scan_code(WORD vk, char *a, int size)
{
	char sc = MapVirtualKeyA((UINT)vk, MAPVK_VK_TO_CHAR);
	if (sc >= '0' && sc <= 'Z')
	{
		strncpy_s(a, size, "", sizeof(""));
		strncpy_s(a, size, &sc, sizeof(sc));
	}
	else
	{
		strncpy_s(a, size, vk_maps[vk], sizeof(vk_maps[vk]));
	}
}

/**
 * Displays the key to which each button is mapped to.
 * Saves the initial key maps in variables that can be changed later if user wants to.
 */
void Preferences::load_keys()
{
	char buffer[256];
	this->temp[ui->xmap->objectName()] =
		SettingsSingleton::instance().gamepadButtons()[GamepadButtons::GamepadButtons_X].vk;
	get_scan_code(
		SettingsSingleton::instance().gamepadButtons()[GamepadButtons::GamepadButtons_X].vk, buffer,
		256);
	this->ui->xmap->setText(QString(buffer));
	/*------------------------------------------------------------*/
	this->temp[ui->ymap->objectName()] =
		SettingsSingleton::instance().gamepadButtons()[GamepadButtons::GamepadButtons_Y].vk;
	get_scan_code(SettingsSingleton::instance().gamepadButtons()[GamepadButtons_Y].vk, buffer, 256);
	this->ui->ymap->setText(QString(buffer));
	/*------------------------------------------------------------*/
	this->temp[ui->amap->objectName()] =
		SettingsSingleton::instance().gamepadButtons()[GamepadButtons::GamepadButtons_A].vk;
	get_scan_code(SettingsSingleton::instance().gamepadButtons()[GamepadButtons_A].vk, buffer, 256);
	this->ui->amap->setText(QString(buffer));
	/*------------------------------------------------------------*/
	this->temp[ui->bmap->objectName()] =
		SettingsSingleton::instance().gamepadButtons()[GamepadButtons::GamepadButtons_B].vk;
	get_scan_code(SettingsSingleton::instance().gamepadButtons()[GamepadButtons_B].vk, buffer, 256);
	this->ui->bmap->setText(QString(buffer));
	/*------------------------------------------------------------*/
	this->temp[ui->Rtmap->objectName()] =
		SettingsSingleton::instance()
			.gamepadButtons()[GamepadButtons::GamepadButtons_RightShoulder]
			.vk;
	get_scan_code(SettingsSingleton::instance().gamepadButtons()[GamepadButtons_RightShoulder].vk,
				  buffer, 256);
	this->ui->Rtmap->setText(QString(buffer));
	/*------------------------------------------------------------*/
	this->temp[ui->Ltmap->objectName()] =
		SettingsSingleton::instance()
			.gamepadButtons()[GamepadButtons::GamepadButtons_LeftShoulder]
			.vk;
	get_scan_code(SettingsSingleton::instance().gamepadButtons()[GamepadButtons_LeftShoulder].vk,
				  buffer, 256);
	this->ui->Ltmap->setText(QString(buffer));
	/*------------------------------------------------------------*/
	this->temp[ui->ddownmap->objectName()] =
		SettingsSingleton::instance().gamepadButtons()[GamepadButtons::GamepadButtons_DPadDown].vk;
	get_scan_code(SettingsSingleton::instance().gamepadButtons()[GamepadButtons_DPadDown].vk,
				  buffer, 256);
	this->ui->ddownmap->setText(QString(buffer));
	/*------------------------------------------------------------*/
	this->temp[ui->dupmap->objectName()] =
		SettingsSingleton::instance().gamepadButtons()[GamepadButtons::GamepadButtons_DPadUp].vk;
	get_scan_code(SettingsSingleton::instance().gamepadButtons()[GamepadButtons_DPadUp].vk, buffer,
				  256);
	this->ui->dupmap->setText(QString(buffer));
	/*------------------------------------------------------------*/
	this->temp[ui->drightmap->objectName()] =
		SettingsSingleton::instance().gamepadButtons()[GamepadButtons::GamepadButtons_DPadRight].vk;
	get_scan_code(SettingsSingleton::instance().gamepadButtons()[GamepadButtons_DPadRight].vk,
				  buffer, 256);
	this->ui->drightmap->setText(QString(buffer));
	/*------------------------------------------------------------*/
	this->temp[ui->dleftmap->objectName()] =
		SettingsSingleton::instance().gamepadButtons()[GamepadButtons::GamepadButtons_DPadLeft].vk;
	get_scan_code(SettingsSingleton::instance().gamepadButtons()[GamepadButtons_DPadLeft].vk,
				  buffer, 256);
	this->ui->dleftmap->setText(QString(buffer));
	/*-----------------------------------------------------------*/
	this->temp[ui->viewmap->objectName()] =
		SettingsSingleton::instance().gamepadButtons()[GamepadButtons::GamepadButtons_View].vk;
	get_scan_code(SettingsSingleton::instance().gamepadButtons()[GamepadButtons_View].vk, buffer,
				  256);
	this->ui->viewmap->setText(QString(buffer));
	/*-----------------------------------------------------------*/
	this->temp[ui->menumap->objectName()] =
		SettingsSingleton::instance().gamepadButtons()[GamepadButtons::GamepadButtons_Menu].vk;
	get_scan_code(SettingsSingleton::instance().gamepadButtons()[GamepadButtons_Menu].vk, buffer,
				  256);
	this->ui->menumap->setText(QString(buffer));
	/*-----------------------------------------------------------*/

	load_thumbsticks();
}

void Preferences::load_thumbstick_keys()
{
	load_thumbsticks();
}

/**
 * The event filter virtual function is redefined to to filter for mouse and keyboard inputs when
 * user tries to change the button-key maps. Checks which object is sending the event and type of
 * event. If event is a keyboard or mouse button press then map and the object is button map then
 * get the virtual key code of the key pressed and store the change in a temporary variable.
 * @param sender
 * To get the address of the object that is triggering the event.
 * @param event
 * To capture the event that was triggered.
 * @return [bool] True if event is handled else False.
 */
bool Preferences::eventFilter(QObject *sender, QEvent *event)
{
	if (QLineEdit *ptr = qobject_cast<QLineEdit *>(sender); ptr)
	{
		if (event->type() == QEvent::KeyPress && ptr->text() == "")
		{
			const QKeyEvent *key_press = (QKeyEvent *)event;
			temp[ptr->objectName()] = key_press->nativeVirtualKey();
			char buffer[256];
			get_scan_code(key_press->nativeVirtualKey(), buffer, 256);
			ptr->setText(QString(buffer));
			return true;
		}
		else if (event->type() == QEvent::MouseButtonPress && ptr->text() == "")
		{
			const QMouseEvent *mouse_press = static_cast<QMouseEvent *>(event);
			char buffer[256];
			bool valid = false;
			UINT button = mouse_press->button();
			switch (button)
			{
			case Qt::MouseButton::LeftButton:
				temp[ptr->objectName()] = VK_LBUTTON;
				get_scan_code(VK_LBUTTON, buffer, 256);
				valid = true;
				break;
			case Qt::MouseButton::RightButton:
				temp[ptr->objectName()] = VK_RBUTTON;
				get_scan_code(VK_RBUTTON, buffer, 256);
				valid = true;
				break;
			case Qt::MouseButton::MiddleButton:
				temp[ptr->objectName()] = VK_MBUTTON;
				get_scan_code(VK_MBUTTON, buffer, 256);
				valid = true;
				break;
			default:
				qWarning() << "No legal mouse button found.";
			}
			if (valid)
				ptr->setText(QString(buffer));
			return true;
		}
		else
		{
			if ((event->type() == QEvent::KeyPress || event->type() == QEvent::MouseButtonPress) &&
				ptr->hasFocus())
				return true;
		}
	}
	return QWidget::eventFilter(sender, event);
}

/**
 * Make the QWidget box ignore the enter key and escape key presses when the focus is on button map
 * @param e
 * Capture the key_press event
 */
void Preferences::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return || e->key() == Qt::Key_Escape)
		return;
	return QWidget::keyPressEvent(e);
}

/**
 * Install the above event filter on all the button maps to capture the key presses when they have
 * the focus.
 */
void Preferences::install_event_filter()
{
	// Use ButtonMaps and ThumbstickMaps group boxes to find QLineEdit children
	QList<QLineEdit *> buttonEdits = ui->ButtonMaps->findChildren<QLineEdit *>();
	for (QLineEdit *edit : buttonEdits)
	{
		edit->installEventFilter(this);
	}

	if (ui->ThumbstickMaps)
	{
		QList<QLineEdit *> thumbEdits = ui->ThumbstickMaps->findChildren<QLineEdit *>();
		for (QLineEdit *edit : thumbEdits)
		{
			edit->installEventFilter(this);
		}
	}
}

void Preferences::change_mouse_sensitivity(int value)
{
	SettingsSingleton::instance().setMouseSensitivity(value);
}

void Preferences::show_help()
{
	QString helpText = tr("The settings file is located at:  \n```\n%1\n```")
						   .arg(SettingsSingleton::instance().qsettings()->fileName());

	QMessageBox helpBox(this);
	helpBox.setWindowTitle("Preferences Help");
	helpBox.setIcon(QMessageBox::Icon::Information);

	helpBox.setText(helpText);
	helpBox.setTextFormat(Qt::TextFormat::MarkdownText);
	helpBox.setStandardButtons(QMessageBox::Ok);
	helpBox.exec();
}
