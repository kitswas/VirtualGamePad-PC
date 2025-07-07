#include "preferences.hpp"

#include "../settings/input_types.hpp"
#include "../settings/settings.hpp"
#include "../settings/settings_singleton.hpp"
#include "buttoninputbox.hpp"
#include "ui_preferences.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QInputDialog>
#include <QMessageBox>
#include <QSlider>
#include <QStandardPaths>

Preferences::Preferences(QWidget *parent) : QWidget(parent), ui(new Ui::Preferences)
{
	qDebug() << "Initializing Preferences dialog";

	ui->setupUi(this);
	setupKeymapTabs();
	ui->pointerSlider->adjustSize();

	connect(ui->buttonBox, &QDialogButtonBox::rejected, this, [this] {
		load_keys();
		this->deleteLater();
	});
	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [this] { this->deleteLater(); });
	connect(ui->buttonBox, &QDialogButtonBox::helpRequested, this, &Preferences::show_help);
	connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked, this,
			&Preferences::restore_defaults);

	ui->buttonBox->setCenterButtons(true);
	ui->pointerSlider->setValue(SettingsSingleton::instance().mouseSensitivity() / 100);
	ui->executorNotesLabel->setOpenExternalLinks(true);

	setup_profile_management();

	// Load preferences into UI
	load_port();
	load_executor_type();

	// Connect executor type radio buttons
	connect(ui->gamepadExecutorRadio, &QRadioButton::toggled, this,
			&Preferences::executor_type_changed);
	connect(ui->keyboardMouseExecutorRadio, &QRadioButton::toggled, this,
			&Preferences::executor_type_changed);

	// No immediate save connections - only save when OK is clicked
	disconnect(ui->buttonBox, &QDialogButtonBox::accepted, nullptr, nullptr);
	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [this] {
		auto &settings = SettingsSingleton::instance();

		// Save mouse sensitivity
		settings.setMouseSensitivity(ui->pointerSlider->value() * 100);

		// Save port number now
		settings.setPort(ui->portSpinBox->value());

		// Save executor type
		ExecutorType executorType = ui->gamepadExecutorRadio->isChecked()
										? ExecutorType::GamepadExecutor
										: ExecutorType::KeyboardMouseExecutor;
		settings.setExecutorType(executorType);

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
	int currentProfileIndex =
		ui->profileComboBox->findText(currentProfile.isEmpty() ? "Default" : currentProfile);
	if (currentProfileIndex >= 0)
	{
		ui->profileComboBox->setCurrentIndex(currentProfileIndex);
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
		qInfo() << "Creating new profile:" << profileName;

		// Check if profile already exists
		if (settings.profileExists(profileName))
		{
			qWarning() << "Profile creation failed - profile already exists:" << profileName;
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
			qInfo() << "Profile created successfully:" << profileName;
			refresh_profile_list();
			ui->profileComboBox->setCurrentText(profileName);
			currentProfile = profileName;

			QMessageBox::information(this, "Profile Created",
									 "Profile '" + profileName + "' created successfully");
		}
		else
		{
			qCritical() << "Failed to create profile:" << profileName;
			QMessageBox::warning(this, "Error", "Failed to create profile '" + profileName + "'");
		}
	}
}

void Preferences::delete_profile()
{
	QString profileName = ui->profileComboBox->currentText();
	qInfo() << "Attempting to delete profile:" << profileName;

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
			qInfo() << "Profile deleted successfully:" << profileName;
			refresh_profile_list();
			QMessageBox::information(this, "Profile Deleted",
									 "Profile '" + profileName + "' deleted successfully");
		}
		else
		{
			qCritical() << "Failed to delete profile:" << profileName;
			QMessageBox::warning(this, "Error", "Failed to delete profile '" + profileName + "'");
		}
	}
}

void Preferences::profile_selection_changed(const QString &profileName)
{
	if (profileName.isEmpty())
		return;

	qDebug() << "Profile selection changed to:" << profileName;

	auto &settings = SettingsSingleton::instance();

	// Discard any changes to the current profile (don't save)
	// Just load the new profile directly

	// Load the selected profile using SettingsSingleton
	if (settings.loadProfile(profileName))
	{
		qInfo() << "Profile loaded successfully:" << profileName;
		currentProfile = profileName;
		// Update UI from the loaded profile's keymaps
		load_keys();
	}
	else
	{
		qCritical() << "Failed to load profile:" << profileName;
		QMessageBox::warning(this, "Error", "Failed to load profile '" + profileName + "'");
	}
}

void Preferences::load_thumbsticks()
{
	auto const &profile = SettingsSingleton::instance().activeKeymapProfile();
	// Left thumbstick
	ui->leftThumbMouseMove->setChecked(profile.leftThumbMouseMove());
	auto left = profile.thumbstickInput(Thumbstick_Left);
	ui->leftThumbUpMap->setKeyCodeAndDisplayName(left.up.vk, left.up.displayName);
	ui->leftThumbDownMap->setKeyCodeAndDisplayName(left.down.vk, left.down.displayName);
	ui->leftThumbLeftMap->setKeyCodeAndDisplayName(left.left.vk, left.left.displayName);
	ui->leftThumbRightMap->setKeyCodeAndDisplayName(left.right.vk, left.right.displayName);

	// Right thumbstick
	ui->rightThumbMouseMove->setChecked(profile.rightThumbMouseMove());
	auto right = profile.thumbstickInput(Thumbstick_Right);
	ui->rightThumbUpMap->setKeyCodeAndDisplayName(right.up.vk, right.up.displayName);
	ui->rightThumbDownMap->setKeyCodeAndDisplayName(right.down.vk, right.down.displayName);
	ui->rightThumbLeftMap->setKeyCodeAndDisplayName(right.left.vk, right.left.displayName);
	ui->rightThumbRightMap->setKeyCodeAndDisplayName(right.right.vk, right.right.displayName);
}

void Preferences::change_key_inputs()
{
	auto &profile = SettingsSingleton::instance().activeKeymapProfile();
	auto getBox = [&](ButtonInputBox const *box, GamepadButtons btn) {
		InputKeyCode vk = box->keyCode();
		QString displayName = box->displayName();
		profile.setButtonInput(btn, vk, displayName);
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
	left.up.displayName = ui->leftThumbUpMap->displayName();
	left.down.vk = ui->leftThumbDownMap->keyCode();
	left.down.displayName = ui->leftThumbDownMap->displayName();
	left.left.vk = ui->leftThumbLeftMap->keyCode();
	left.left.displayName = ui->leftThumbLeftMap->displayName();
	left.right.vk = ui->leftThumbRightMap->keyCode();
	left.right.displayName = ui->leftThumbRightMap->displayName();
	profile.setThumbstickInput(Thumbstick_Left, left);

	// Right thumbstick
	ThumbstickInput right;
	right.is_mouse_move = ui->rightThumbMouseMove->isChecked();
	right.up.vk = ui->rightThumbUpMap->keyCode();
	right.up.displayName = ui->rightThumbUpMap->displayName();
	right.down.vk = ui->rightThumbDownMap->keyCode();
	right.down.displayName = ui->rightThumbDownMap->displayName();
	right.left.vk = ui->rightThumbLeftMap->keyCode();
	right.left.displayName = ui->rightThumbLeftMap->displayName();
	right.right.vk = ui->rightThumbRightMap->keyCode();
	right.right.displayName = ui->rightThumbRightMap->displayName();
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
		box->setKeyCodeAndDisplayName(profile.buttonMap(btn), profile.buttonDisplayName(btn));
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
	QString helpText = tr(R"(
Changes are saved to disk only when you press Ok.  
Restore Defaults loads defaults for all settings (including the active profile).

The settings file is located at:  
[`%1`](file:///%1)

You can switch between different keymaps for different games using Profiles.  
You can share profiles with others (same OS) by copying the files in the Profiles directory at:  
[`%2`](file:///%2)

Download sample profiles from: <https://gist.github.com/kitswas/b7a100954de7dd7dcbe52cd38a27c8cf>
)")
						   .arg(SettingsSingleton::instance().qsettings()->fileName())
						   .arg(SettingsSingleton::instance().getProfilesDir());

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
	if (SettingsSingleton::DEFAULT_EXECUTOR_TYPE == ExecutorType::GamepadExecutor)
	{
		ui->gamepadExecutorRadio->setChecked(true);
	}
	else
	{
		ui->keyboardMouseExecutorRadio->setChecked(true);
	}

	// Refresh UI to show the default values
	load_keys();
	load_port();
	load_executor_type();

	QMessageBox::information(this, "Defaults Restored",
							 "Default settings have been applied.\n"
							 "Press OK in the preferences dialog to save these changes.");
}

void Preferences::load_executor_type()
{
	ExecutorType executorType = SettingsSingleton::instance().executorType();

	if (executorType == ExecutorType::GamepadExecutor)
	{
		ui->gamepadExecutorRadio->setChecked(true);
	}
	else
	{
		ui->keyboardMouseExecutorRadio->setChecked(true);
	}

	update_executor_notes();
}

void Preferences::executor_type_changed()
{
	update_executor_notes();
}

void Preferences::update_executor_notes()
{
	QString notes;
	auto linux_common_notes =
		"Requires access to /dev/uinput. "
		"uinput kernel module must be loaded.\n"
		"\nQuick and dirty access (not recommended for security):\n"
		"- run the application with sudo\n"
		"- Or, `sudo chmod 666 /dev/uinput`\n"
		"\nRecommended way:\n"
		"1. Create the uinput group if it doesn't exist: sudo groupadd uinput\n"
		"2. Add your user to the uinput group: sudo usermod -aG uinput $USER\n"
		"3. Make it persistent with this command:\n"
		" ``` sudo sh -c 'echo KERNEL==\\\"uinput\\\", SUBSYSTEM==\\\"misc\\\", MODE=\\\"0660\\\", "
		"GROUP=\\\"uinput\\\" > /etc/udev/rules.d/99-uinput.rules' ```\n"
		"4. Log out and back in for the group changes to take effect.";

#if defined(__linux__)
	// Test for access
	if (access("/dev/uinput", R_OK | W_OK) != 0)
	{
		notes = linux_common_notes;
	}
	else
	{
		notes = "RW access to /dev/uinput is available on this system. No action required.";
	}
#endif

	if (ui->gamepadExecutorRadio->isChecked())
	{
		// notes for GamepadExecutor
#ifdef _WIN32
		notes = "notes for Windows:\n"
				"- Windows 10 or later\n"
				"- Administrator privileges\n"
				"- Enable [app sideloading (developer mode) in Settings](ms-settings:developers)\n"
				"- Creates a virtual gamepad device\n"
				"- **Experimental**: "
				"[May crash anytime.](https://github.com/microsoft/microsoft-ui-xaml/issues/8639)";
#elif defined(__linux__)
#else
		notes = "Platform-specific notes apply for virtual gamepad creation.";
#endif
	}
	else
	{
		// notes for KeyboardMouseExecutor
#ifdef _WIN32
		notes = "notes for Windows:\n"
				"- Windows 7 or later\n"
				"- No special privileges required\n"
				"- Simulates keyboard and mouse input\n"
				"- Uses configurable key mappings";
#elif defined(__linux__)
#else
		notes = "Simulates keyboard and mouse input using configurable key mappings.";
#endif
	}

	ui->executorNotesLabel->setText(notes);
}
