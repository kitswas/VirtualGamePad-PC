#include "preferences.hpp"

#include "../settings/settings.hpp"
#include "../settings/settings_singleton.hpp"
#include "ButtonInputBox.hpp"
#include "ui_preferences.h"
#include "winuser.h"

#include <QDebug>
#include <QDir>
#include <QFile>
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
	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [this] { this->deleteLater(); });
	connect(ui->buttonBox, &QDialogButtonBox::helpRequested, this, &Preferences::show_help);
	connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked, this,
			&Preferences::restore_defaults);

	ui->buttonBox->setCenterButtons(true);
	ui->pointerSlider->setValue(SettingsSingleton::instance().mouseSensitivity() / 100);

	setup_profile_management();

	// Load preferences into UI
	load_port();

	// No immediate save connections - only save when OK is clicked
	disconnect(ui->buttonBox, &QDialogButtonBox::accepted, nullptr, nullptr);
	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [this] {
		auto &settings = SettingsSingleton::instance();

		// Save mouse sensitivity
		settings.setMouseSensitivity(ui->pointerSlider->value() * 100);

		// Save port number now
		settings.setPort(ui->portSpinBox->value());

		// Update key mapping in the active profile
		change_key_inputs();

		// Update and save the active profile
		currentProfile = ui->profileComboBox->currentText();
		settings.setActiveProfileName(currentProfile);
		settings.saveActiveProfile();

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
	auto const &settings = SettingsSingleton::instance();

	// Connect buttons
	connect(ui->newProfileButton, &QPushButton::clicked, this, &Preferences::new_profile);
	connect(ui->deleteProfileButton, &QPushButton::clicked, this, &Preferences::delete_profile);
	connect(ui->profileComboBox, &QComboBox::currentTextChanged, this,
			&Preferences::profile_selection_changed);

	// Initialize with available profiles
	refresh_profile_list();

	// Set active profile
	currentProfile = settings.activeProfileName();
	int defaultIndex =
		ui->profileComboBox->findText(currentProfile.isEmpty() ? "Default" : currentProfile);
	if (defaultIndex >= 0)
	{
		ui->profileComboBox->setCurrentIndex(defaultIndex);
	}

	// Explicitly trigger the profile selection change to load the profile
	profile_selection_changed(ui->profileComboBox->currentText());
}

void Preferences::refresh_profile_list()
{
	ui->profileComboBox->blockSignals(true);
	ui->profileComboBox->clear();

	QStringList profiles = SettingsSingleton::instance().listAvailableProfiles();
	ui->profileComboBox->addItems(profiles);

	ui->profileComboBox->blockSignals(false);
}

void Preferences::new_profile()
{
	bool ok;
	QString profileName = QInputDialog::getText(this, "New Profile",
												"Enter profile name:", QLineEdit::Normal, "", &ok);
	auto &settings = SettingsSingleton::instance();

	if (ok && !profileName.isEmpty())
	{
		// Check if profile already exists
		if (settings.profileExists(profileName))
		{
			QMessageBox::warning(
				this, "Profile Exists",
				"A profile with this name already exists. Please choose a different name.");
			return;
		}

		// Ensure current UI values are applied to the active profile
		change_key_inputs();

		// Save current settings as new profile
		if (settings.createProfile(profileName))
		{
			refresh_profile_list();
			ui->profileComboBox->setCurrentText(profileName);
			currentProfile = profileName;

			QMessageBox::information(this, "Profile Created",
									 "Profile '" + profileName + "' created successfully");
		}
		else
		{
			QMessageBox::warning(this, "Error", "Failed to create profile '" + profileName + "'");
		}
	}
}

void Preferences::delete_profile()
{
	QString profileName = ui->profileComboBox->currentText();
	if (profileName.isEmpty())
	{
		return;
	}

	auto &settings = SettingsSingleton::instance();

	// Don't delete active profile
	if (profileName == settings.activeProfileName())
	{
		QMessageBox::warning(this, "Cannot Delete",
							 "Cannot delete the active profile. Switch to another profile first.");
		return;
	}

	// Confirm deletion
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, "Delete Profile",
								  "Are you sure you want to delete profile '" + profileName + "'?",
								  QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::Yes)
	{
		if (settings.deleteProfile(profileName))
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

	auto &settings = SettingsSingleton::instance();

	// Discard any changes to the current profile (don't save)
	// Just load the new profile directly

	// Load the selected profile using SettingsSingleton
	if (settings.loadProfile(profileName))
	{
		currentProfile = profileName;
		// Update UI from the loaded profile's keymaps
		load_keys();
	}
	else
	{
		QMessageBox::warning(this, "Error", "Failed to load profile '" + profileName + "'");
	}
}

void Preferences::load_thumbsticks()
{
	auto const &profile = SettingsSingleton::instance().activeKeymapProfile();
	// Left thumbstick
	ui->leftThumbMouseMove->setChecked(profile.leftThumbMouseMove());
	auto left = profile.thumbstickInput(Thumbstick_Left);
	ui->leftThumbUpMap->setKeyCode(left.up.vk);
	ui->leftThumbDownMap->setKeyCode(left.down.vk);
	ui->leftThumbLeftMap->setKeyCode(left.left.vk);
	ui->leftThumbRightMap->setKeyCode(left.right.vk);

	// Right thumbstick
	ui->rightThumbMouseMove->setChecked(profile.rightThumbMouseMove());
	auto right = profile.thumbstickInput(Thumbstick_Right);
	ui->rightThumbUpMap->setKeyCode(right.up.vk);
	ui->rightThumbDownMap->setKeyCode(right.down.vk);
	ui->rightThumbLeftMap->setKeyCode(right.left.vk);
	ui->rightThumbRightMap->setKeyCode(right.right.vk);
}

void Preferences::change_key_inputs()
{
	auto &profile = SettingsSingleton::instance().activeKeymapProfile();
	auto getBox = [&](ButtonInputBox const *box, GamepadButtons btn) {
		WORD vk = box->keyCode();
		profile.setButtonMap(btn, vk);
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
	auto &profile = SettingsSingleton::instance().activeKeymapProfile();
	// Left thumbstick
	ThumbstickInput left;
	left.is_mouse_move = ui->leftThumbMouseMove->isChecked();
	left.up.vk = ui->leftThumbUpMap->keyCode();
	left.down.vk = ui->leftThumbDownMap->keyCode();
	left.left.vk = ui->leftThumbLeftMap->keyCode();
	left.right.vk = ui->leftThumbRightMap->keyCode();
	profile.setThumbstickInput(Thumbstick_Left, left);

	// Right thumbstick
	ThumbstickInput right;
	right.is_mouse_move = ui->rightThumbMouseMove->isChecked();
	right.up.vk = ui->rightThumbUpMap->keyCode();
	right.down.vk = ui->rightThumbDownMap->keyCode();
	right.left.vk = ui->rightThumbLeftMap->keyCode();
	right.right.vk = ui->rightThumbRightMap->keyCode();
	profile.setThumbstickInput(Thumbstick_Right, right);
}

/**
 * Displays the key to which each button is mapped to.
 * Saves the initial key maps in variables that can be changed later if user wants to.
 */
void Preferences::load_keys()
{
	auto const &profile = SettingsSingleton::instance().activeKeymapProfile();
	auto setBox = [&](ButtonInputBox *box, GamepadButtons btn) {
		box->setKeyCode(profile.buttonMap(btn));
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

void Preferences::restore_defaults()
{
	// Show confirmation dialog
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(
		this, "Restore Defaults",
		"Are you sure you want to restore all settings to their default values?",
		QMessageBox::Yes | QMessageBox::No);

	if (reply != QMessageBox::Yes)
		return;

	// Delegate to SettingsSingleton to reset the settings
	auto &settings = SettingsSingleton::instance();
	settings.resetToDefaults();

	// Update UI to reflect the default values
	ui->pointerSlider->setValue(SettingsSingleton::DEFAULT_MOUSE_SENSITIVITY);
	ui->portSpinBox->setValue(SettingsSingleton::DEFAULT_PORT_NUMBER);

	// Refresh UI to show the default values
	load_keys();
	load_port();

	QMessageBox::information(this, "Defaults Restored",
							 "Default settings have been applied.\n"
							 "Press OK in the preferences dialog to save these changes.");
}
