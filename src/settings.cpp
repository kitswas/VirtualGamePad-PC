#include "settings.h"
#include "settings_key_variables.h"
#include <QDebug>

using namespace setting_keys;

const QString SETTINGS_FILE = QDir::toNativeSeparators(
	QDir::homePath() + "//VirtualGamePad.ini"); // the path of the settigs file. C:\Users\<username>\VirtualGamePad.ini
QSettings *settings;

QString setting_keys::Mouse_sensivity = "mouse_setting/mouse_sensivity";

void (*load_functions[3])(void) = {
	load_mouse_setting, load_port_number,
	load_key_maps}; // an array of pointer to functions that needs to run on startup to load settings.

/**
 * @brief save_setting
 * Save a setting to the settings file
 * @param key
 * The name of the setting in string format if you want to group settings seperate the head group using </>
 * @param value
 * The value of the settings can be of any data type string, int, char, float, e.t.c.
 */
void save_setting(QString key, QVariant value)
{
	settings->setValue(key, value);
	settings->sync();
	qDebug() << settings->value(key).toString();
	qDebug() << settings->fileName();
}

/**
 * @brief load_setting
 * Get the value of a setting.
 * @param key
 * The fullname including the groups of the setting in string format
 * @return The setting as QVariant can be converted into almost all data types.
 */
QVariant load_setting(QString key)
{
	QVariant value = settings->value(key);
	return value;
}

/**
 * @brief load_mouse_setting
 * Loads and sets the mouse sensivity.
 */
void load_mouse_setting()
{
	mouse_sensivity = settings->value(setting_keys::Mouse_sensivity).toInt() * 100;
}

/**
 * @brief load_settings_file
 * Loads the settings file for the current application.
 * @param parent
 * The parent object either a widget or Qt Application e.t.c
 */
void load_settings_file(QObject *parent = nullptr)
{
	settings = new QSettings(SETTINGS_FILE, QSettings::Format::IniFormat, parent);
}

/**
 * @brief load_port_number
 * Load and set the port number
 */
void load_port_number() // set the port number as user_defined
{
	port = settings->value(server_settings[setting_keys::Port]).toInt();
}

/**
 * @brief load_key_maps
 * Load the key maps from the settings file.
 */
void load_key_maps() // set the key mappings to the stored values
{
	GAMEPAD_BUTTONS[GamepadButtons_A] =
		TRIAL{(WORD)settings->value(keymaps[A], GAMEPAD_BUTTONS[GamepadButtons_A].vk).toULongLong(),
			  is_mouse_button(settings->value(keymaps[A], GAMEPAD_BUTTONS[GamepadButtons_A].vk).toULongLong())};
	GAMEPAD_BUTTONS[GamepadButtons_B] =
		TRIAL{(WORD)settings->value(keymaps[B], GAMEPAD_BUTTONS[GamepadButtons_B].vk).toULongLong(),
			  is_mouse_button(settings->value(keymaps[B], GAMEPAD_BUTTONS[GamepadButtons_B].vk).toULongLong())};
	GAMEPAD_BUTTONS[GamepadButtons_X] =
		TRIAL{(WORD)settings->value(keymaps[X], GAMEPAD_BUTTONS[GamepadButtons_X].vk).toULongLong(),
			  is_mouse_button(settings->value(keymaps[X], GAMEPAD_BUTTONS[GamepadButtons_X].vk).toULongLong())};
	GAMEPAD_BUTTONS[GamepadButtons_Y] =
		TRIAL{(WORD)settings->value(keymaps[Y], GAMEPAD_BUTTONS[GamepadButtons_Y].vk).toULongLong(),
			  is_mouse_button(settings->value(keymaps[Y], GAMEPAD_BUTTONS[GamepadButtons_Y].vk).toULongLong())};
	GAMEPAD_BUTTONS[GamepadButtons_LeftShoulder] =
		TRIAL{(WORD)settings->value(keymaps[LSHDR], GAMEPAD_BUTTONS[GamepadButtons_LeftShoulder].vk).toULongLong(),
			  is_mouse_button(
				  settings->value(keymaps[LSHDR], GAMEPAD_BUTTONS[GamepadButtons_LeftShoulder].vk).toULongLong())};
	GAMEPAD_BUTTONS[GamepadButtons_RightShoulder] =
		TRIAL{(WORD)settings->value(keymaps[RSHDR], GAMEPAD_BUTTONS[GamepadButtons_RightShoulder].vk).toULongLong(),
			  is_mouse_button(
				  settings->value(keymaps[RSHDR], GAMEPAD_BUTTONS[GamepadButtons_RightShoulder].vk).toULongLong())};
	GAMEPAD_BUTTONS[GamepadButtons_DPadDown] = TRIAL{
		(WORD)settings->value(keymaps[DPADDOWN], GAMEPAD_BUTTONS[GamepadButtons_DPadDown].vk).toULongLong(),
		is_mouse_button(settings->value(keymaps[DPADDOWN], GAMEPAD_BUTTONS[GamepadButtons_DPadDown].vk).toULongLong())};
	GAMEPAD_BUTTONS[GamepadButtons_DPadUp] = TRIAL{
		(WORD)settings->value(keymaps[DPADUP], GAMEPAD_BUTTONS[GamepadButtons_DPadUp].vk).toLongLong(),
		is_mouse_button(settings->value(keymaps[DPADUP], GAMEPAD_BUTTONS[GamepadButtons_DPadUp].vk).toLongLong())};
	GAMEPAD_BUTTONS[GamepadButtons_DPadRight] =
		TRIAL{(WORD)settings->value(keymaps[DPADRIGHT], GAMEPAD_BUTTONS[GamepadButtons_DPadRight].vk).toLongLong(),
			  is_mouse_button(
				  settings->value(keymaps[DPADRIGHT], GAMEPAD_BUTTONS[GamepadButtons_DPadRight].vk).toLongLong())};
	GAMEPAD_BUTTONS[GamepadButtons_DPadLeft] = TRIAL{
		(WORD)settings->value(keymaps[DPADLEFT], GAMEPAD_BUTTONS[GamepadButtons_DPadLeft].vk).toLongLong(),
		is_mouse_button(settings->value(keymaps[DPADLEFT], GAMEPAD_BUTTONS[GamepadButtons_DPadLeft].vk).toLongLong())};
}

/**
 * @brief load_all_settings
 * Run the functions using the function array to load all the necessary settings.
 */
void load_all_settings()
{
	for (int i = 0; i < 3; i++)
	{
		(*load_functions[i])();
	}
}

/**
 * @brief is_mouse_button
 * Function to find out if the user mapped a keyboard key or mouse button
 * @param vk
 * Virtual key code of the key or mouse button pressed
 * @return 1 if its a mouse button else 0
 */
uint is_mouse_button(UINT vk)
{
	for (uint i = 0; i < 3; ++i)
	{
		if (MOUSE_BUTTONS[i] == vk)
			return 1;
	}
	return 0;
}
