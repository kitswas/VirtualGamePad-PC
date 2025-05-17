#include "preferences.hpp"

#include "../settings/settings.hpp"
#include "../settings/settings_singleton.hpp"
#include "ButtonInputBox.hpp"
#include "ui_preferences.h"
#include "winuser.h"

#include <QDebug>
#include <QDir>
#include <QInputDialog>
#include <QMessageBox>
#include <QSlider>
#include <QStandardPaths>

Preferences::Preferences(QWidget *parent) : QWidget(parent), ui(new Ui::Preferences)
{
	ui->setupUi(this);
	setupKeymapTabs();
	ui->pointerSlider->adjustSize();

	ui->buttonBox->connect(ui->buttonBox, &QDialogButtonBox::rejected, this, [this] {
		load_keys();
		this->deleteLater();
	});
	ui->buttonBox->connect(ui->buttonBox, &QDialogButtonBox::accepted, this,
						   [this] { this->deleteLater(); });
	ui->buttonBox->connect(ui->buttonBox, &QDialogButtonBox::helpRequested, this,
						   &Preferences::show_help);
	ui->buttonBox->setCenterButtons(true);
	ui->pointerSlider->setValue(SettingsSingleton::instance().mouseSensitivity() / 100);

	setup_profile_management();

	// Connect port spin box
	connect(ui->portSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
			&Preferences::change_port);

	// Load preferences
	load_port();

	ui->buttonBox->connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [this] {
		SettingsSingleton::instance().setMouseSensitivity(ui->pointerSlider->value() * 100);
		qDebug() << SettingsSingleton::instance().mouseSensitivity();

		// Only update in-memory key/axis mappings, do NOT save to VirtualGamePad.ini.
		change_key_inputs();

		// Auto-save current profile when OK is clicked
		currentProfile = ui->profileComboBox->currentText();
		QString profilePath = get_profiles_dir() + "/" + currentProfile + ".ini";
		save_active_profile_name(currentProfile);
		save_profile_to_file(profilePath);

		this->deleteLater();
	});
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

	// Connect buttons
	connect(ui->newProfileButton, &QPushButton::clicked, this, &Preferences::new_profile);
	connect(ui->deleteProfileButton, &QPushButton::clicked, this, &Preferences::delete_profile);
	connect(ui->profileComboBox, &QComboBox::currentTextChanged, this,
			&Preferences::profile_selection_changed);

	// Initialize with available profiles
	refresh_profile_list();

	// Set default profile as "Default" if it exists, or load last used profile
	currentProfile = load_active_profile_name();
	int defaultIndex =
		ui->profileComboBox->findText(currentProfile.isEmpty() ? "Default" : currentProfile);
	if (defaultIndex >= 0)
	{
		ui->profileComboBox->setCurrentIndex(defaultIndex);
	}

	// Explicitly trigger the profile selection change to load the profile
	profile_selection_changed(ui->profileComboBox->currentText());
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

bool Preferences::load_profile_from_file(const QString &profilePath)
{
	QSettings profileSettings(profilePath, QSettings::IniFormat);

	// Read key mappings
	for (auto it = button_settings.begin(); it != button_settings.end(); ++it)
	{
		QString settingKey = it.value();
		int vk = profileSettings.value(settingKey, -1).toInt();

		if (vk != -1)
		{
			QString objName;

			// Map setting key to UI element name
			if (settingKey == button_settings[setting_keys::button_keys::X])
				objName = "xmap";
			else if (settingKey == button_settings[setting_keys::button_keys::Y])
				objName = "ymap";
			else if (settingKey == button_settings[setting_keys::button_keys::A])
				objName = "amap";
			else if (settingKey == button_settings[setting_keys::button_keys::B])
				objName = "bmap";
			else if (settingKey == button_settings[setting_keys::button_keys::LSHDR])
				objName = "Ltmap";
			else if (settingKey == button_settings[setting_keys::button_keys::RSHDR])
				objName = "Rtmap";
			else if (settingKey == button_settings[setting_keys::button_keys::DPADDOWN])
				objName = "ddownmap";
			else if (settingKey == button_settings[setting_keys::button_keys::DPADUP])
				objName = "dupmap";
			else if (settingKey == button_settings[setting_keys::button_keys::DPADLEFT])
				objName = "dleftmap";
			else if (settingKey == button_settings[setting_keys::button_keys::DPADRIGHT])
				objName = "drightmap";
			else if (settingKey == button_settings[setting_keys::button_keys::VIEW])
				objName = "viewmap";
			else if (settingKey == button_settings[setting_keys::button_keys::MENU])
				objName = "menumap";

			if (!objName.isEmpty())
			{
				// Update widget with new key code
				auto box = findChild<ButtonInputBox *>(objName);
				if (box)
					box->setKeyCode((WORD)vk);
			}
		}
	}
	// Read thumbstick mappings
	bool leftThumbMouseMove =
		profileSettings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstick], false)
			.toBool();
	ui->leftThumbMouseMove->setChecked(leftThumbMouseMove);

	ui->leftThumbUpMap->setKeyCode(
		profileSettings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickUpKey], 'W')
			.toInt());

	ui->leftThumbDownMap->setKeyCode(
		profileSettings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickDownKey], 'S')
			.toInt());

	ui->leftThumbLeftMap->setKeyCode(
		profileSettings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickLeftKey], 'A')
			.toInt());

	ui->leftThumbRightMap->setKeyCode(
		profileSettings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickRightKey], 'D')
			.toInt());

	bool rightThumbMouseMove =
		profileSettings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstick], true)
			.toBool();
	ui->rightThumbMouseMove->setChecked(rightThumbMouseMove);

	ui->rightThumbUpMap->setKeyCode(
		profileSettings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickUpKey], VK_UP)
			.toInt());

	ui->rightThumbDownMap->setKeyCode(
		profileSettings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickDownKey],
				   VK_DOWN)
			.toInt());

	ui->rightThumbLeftMap->setKeyCode(
		profileSettings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickLeftKey],
				   VK_LEFT)
			.toInt());

	ui->rightThumbRightMap->setKeyCode(
		profileSettings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickRightKey],
				   VK_RIGHT)
			.toInt());

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
	save_active_profile_name(profileName); // Save as active profile
	QMessageBox::information(this, "Profile Saved",
							 "Profile '" + profileName + "' saved successfully to " + profilePath);
}

void Preferences::save_profile_to_file(const QString &profilePath)
{
	QSettings profileSettings(profilePath, QSettings::IniFormat);

	// Save button mappings
	profileSettings.setValue(button_settings[setting_keys::button_keys::X], ui->xmap->keyCode());
	profileSettings.setValue(button_settings[setting_keys::button_keys::Y], ui->ymap->keyCode());
	profileSettings.setValue(button_settings[setting_keys::button_keys::A], ui->amap->keyCode());
	profileSettings.setValue(button_settings[setting_keys::button_keys::B], ui->bmap->keyCode());
	profileSettings.setValue(button_settings[setting_keys::button_keys::LSHDR],
							 ui->Ltmap->keyCode());
	profileSettings.setValue(button_settings[setting_keys::button_keys::RSHDR],
							 ui->Rtmap->keyCode());
	profileSettings.setValue(button_settings[setting_keys::button_keys::DPADDOWN],
							 ui->ddownmap->keyCode());
	profileSettings.setValue(button_settings[setting_keys::button_keys::DPADUP],
							 ui->dupmap->keyCode());
	profileSettings.setValue(button_settings[setting_keys::button_keys::DPADLEFT],
							 ui->dleftmap->keyCode());
	profileSettings.setValue(button_settings[setting_keys::button_keys::DPADRIGHT],
							 ui->drightmap->keyCode());
	profileSettings.setValue(button_settings[setting_keys::button_keys::VIEW],
							 ui->viewmap->keyCode());
	profileSettings.setValue(button_settings[setting_keys::button_keys::MENU],
							 ui->menumap->keyCode());

	// Save thumbstick mappings
	profileSettings.setValue(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstick],
							 ui->leftThumbMouseMove->isChecked());
	profileSettings.setValue(
		thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickUpKey],
		ui->leftThumbUpMap->keyCode());
	profileSettings.setValue(
		thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickDownKey],
		ui->leftThumbDownMap->keyCode());
	profileSettings.setValue(
		thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickLeftKey],
		ui->leftThumbLeftMap->keyCode());
	profileSettings.setValue(
		thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickRightKey],
		ui->leftThumbRightMap->keyCode());

	profileSettings.setValue(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstick],
							 ui->rightThumbMouseMove->isChecked());
	profileSettings.setValue(
		thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickUpKey],
		ui->rightThumbUpMap->keyCode());
	profileSettings.setValue(
		thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickDownKey],
		ui->rightThumbDownMap->keyCode());
	profileSettings.setValue(
		thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickLeftKey],
		ui->rightThumbLeftMap->keyCode());
	profileSettings.setValue(
		thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickRightKey],
		ui->rightThumbRightMap->keyCode());

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
		return;

	QString profilePath = get_profiles_dir() + "/" + profileName + ".ini";
	if (load_profile_from_file(profilePath))
	{
		currentProfile = profileName;
		save_active_profile_name(profileName);
		// Optionally show a status message, but do not popup a dialog
	}
	else
	{
		QMessageBox::warning(this, "Error", "Failed to load profile '" + profileName + "'");
	}
}

// Save/load active profile name to main settings file
void Preferences::save_active_profile_name(const QString &profileName)
{
	QSettings mainSettings(
		QDir::toNativeSeparators(qApp->applicationDirPath() + "/VirtualGamePad.ini"),
		QSettings::IniFormat);
	mainSettings.setValue("profiles/active", profileName);
	mainSettings.sync();
}

QString Preferences::load_active_profile_name()
{
	QSettings mainSettings(
		QDir::toNativeSeparators(qApp->applicationDirPath() + "/VirtualGamePad.ini"),
		QSettings::IniFormat);
	return mainSettings.value("profiles/active", "Default").toString();
}

void Preferences::load_thumbsticks()
{
	auto &settings_singleton = SettingsSingleton::instance();
	auto &thumbsticks = settings_singleton.thumbstickInputs();

	// Left thumbstick
	ui->leftThumbMouseMove->setChecked(thumbsticks[Thumbstick_Left].is_mouse_move);

	ui->leftThumbUpMap->setKeyCode(thumbsticks[Thumbstick_Left].up.vk);
	ui->leftThumbDownMap->setKeyCode(thumbsticks[Thumbstick_Left].down.vk);
	ui->leftThumbLeftMap->setKeyCode(thumbsticks[Thumbstick_Left].left.vk);
	ui->leftThumbRightMap->setKeyCode(thumbsticks[Thumbstick_Left].right.vk);

	// Right thumbstick
	ui->rightThumbMouseMove->setChecked(thumbsticks[Thumbstick_Right].is_mouse_move);

	ui->rightThumbUpMap->setKeyCode(thumbsticks[Thumbstick_Right].up.vk);
	ui->rightThumbDownMap->setKeyCode(thumbsticks[Thumbstick_Right].down.vk);
	ui->rightThumbLeftMap->setKeyCode(thumbsticks[Thumbstick_Right].left.vk);
	ui->rightThumbRightMap->setKeyCode(thumbsticks[Thumbstick_Right].right.vk);
}

void Preferences::change_key_inputs()
{
	auto &buttons = SettingsSingleton::instance().gamepadButtons();
	auto getBox = [&](ButtonInputBox *box, GamepadButtons btn) {
		WORD vk = box->keyCode();
		buttons[btn].vk = vk;
		buttons[btn].is_mouse_button = is_mouse_button(vk);
		// No saveSetting call here - keymaps should only be saved to profile files
	};
	getBox(ui->xmap, GamepadButtons_X);
	getBox(ui->ymap, GamepadButtons_Y);
	getBox(ui->amap, GamepadButtons_A);
	getBox(ui->bmap, GamepadButtons_B);
	getBox(ui->Ltmap, GamepadButtons_LeftShoulder);
	getBox(ui->Rtmap, GamepadButtons_RightShoulder);
	getBox(ui->ddownmap, GamepadButtons_DPadDown);
	getBox(ui->dupmap, GamepadButtons_DPadUp);
	getBox(ui->drightmap, GamepadButtons_DPadRight);
	getBox(ui->dleftmap, GamepadButtons_DPadLeft);
	getBox(ui->viewmap, GamepadButtons_View);
	getBox(ui->menumap, GamepadButtons_Menu);
	change_thumbstick_inputs();
}

void Preferences::change_thumbstick_inputs()
{
	auto &settings_singleton = SettingsSingleton::instance();
	auto &thumbsticks = settings_singleton.thumbstickInputs();

	// Left thumbstick
	thumbsticks[Thumbstick_Left].is_mouse_move = ui->leftThumbMouseMove->isChecked();

	thumbsticks[Thumbstick_Left].up.vk = ui->leftThumbUpMap->keyCode();
	thumbsticks[Thumbstick_Left].down.vk = ui->leftThumbDownMap->keyCode();
	thumbsticks[Thumbstick_Left].left.vk = ui->leftThumbLeftMap->keyCode();
	thumbsticks[Thumbstick_Left].right.vk = ui->leftThumbRightMap->keyCode();

	settings_singleton.setThumbstickInput(Thumbstick_Left, thumbsticks[Thumbstick_Left]);

	// Right thumbstick
	thumbsticks[Thumbstick_Right].is_mouse_move = ui->rightThumbMouseMove->isChecked();

	thumbsticks[Thumbstick_Right].up.vk = ui->rightThumbUpMap->keyCode();
	thumbsticks[Thumbstick_Right].down.vk = ui->rightThumbDownMap->keyCode();
	thumbsticks[Thumbstick_Right].left.vk = ui->rightThumbLeftMap->keyCode();
	thumbsticks[Thumbstick_Right].right.vk = ui->rightThumbRightMap->keyCode();

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
	auto setBox = [&](ButtonInputBox *box, GamepadButtons btn) {
		WORD vk = SettingsSingleton::instance().gamepadButtons()[btn].vk;
		box->setKeyCode(vk);
	};
	setBox(ui->xmap, GamepadButtons_X);
	setBox(ui->ymap, GamepadButtons_Y);
	setBox(ui->amap, GamepadButtons_A);
	setBox(ui->bmap, GamepadButtons_B);
	setBox(ui->Rtmap, GamepadButtons_RightShoulder);
	setBox(ui->Ltmap, GamepadButtons_LeftShoulder);
	setBox(ui->ddownmap, GamepadButtons_DPadDown);
	setBox(ui->dupmap, GamepadButtons_DPadUp);
	setBox(ui->drightmap, GamepadButtons_DPadRight);
	setBox(ui->dleftmap, GamepadButtons_DPadLeft);
	setBox(ui->viewmap, GamepadButtons_View);
	setBox(ui->menumap, GamepadButtons_Menu);
	load_thumbsticks();
}

void Preferences::load_thumbstick_keys()
{
	load_thumbsticks();
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

void Preferences::load_port()
{
	int port = SettingsSingleton::instance().port();
	ui->portSpinBox->setValue(port);
}

void Preferences::change_port(int value)
{
	SettingsSingleton::instance().setPort(value);
}
