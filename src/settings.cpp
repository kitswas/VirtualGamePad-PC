#include "settings.h"
#include "settings_key_variables.h"
#include <QDebug>

QString SETTINGS_FILE = QDir::toNativeSeparators(QDir::homePath() + "//VirtualGamePad.ini"); // the path of the settigs file. C:\Users\<username>\VirtualGamePad.ini
QSettings *settings;

QString setting_keys::Mouse_sensivity = "mouse_setting/mouse_sensivity";

void (*load_functions[3])(void) = {load_mouse_setting, load_port_number, load_key_maps}; // an array of pointer to functions that needs to run on startup to load settings.

/**
 * @brief save_setting
 * save a setting to the settings file
 * @param key
 * the name of the setting in string format if you want to group settings seperate the head group using </>
 * @param value
 * the value of the settings can be of any data type string, int, char, float, e.t.c.
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
 * get the value of a setting.
 * @param key
 * the fullname including the groups of the setting in string format
 * @return
 * the setting as QVariant can be converted into almost all data types.
 */
QVariant load_setting(QString key)
{
	QVariant value = settings->value(key);
	return value;
}

/**
 * @brief load_mouse_setting
 * loads and sets the mouse sensivity.
 */
void load_mouse_setting()
{
    mouse_sensivity = settings->value(setting_keys::Mouse_sensivity).toInt() * 100;
}

/**
 * @brief load_settings_file
 * loads the settings file for the current application.
 * @param parent
 * the parent object either a widget or Qt Application e.t.c
 */
void load_settings_file(QObject *parent = nullptr)
{
	settings = new QSettings(SETTINGS_FILE, QSettings::Format::IniFormat, parent);
}

/**
 * @brief load_port_number
 * load and set the port number
 */
void load_port_number() // set the port number as user_defined
{
    port = settings->value(server_settings[setting_keys::Port]).toInt();
}

/**
 * @brief load_key_maps
 * load the key maps from the settings file.
 */
void load_key_maps() // set the key mappings to the stored values
{
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_A] = (WORD)settings->value(keymaps[setting_keys::keys::A], GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_A]).toULongLong();
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_B] = (WORD)settings->value(keymaps[setting_keys::keys::B], GAMEPAD_BUTTONS[GamepadButtons_B]).toULongLong();
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_X] = (WORD)settings->value(keymaps[setting_keys::keys::X], GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_X]).toULongLong();
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_Y] = (WORD)settings->value(keymaps[setting_keys::keys::Y], GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_Y]).toULongLong();
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_LeftThumbstick] = (WORD)settings->value(keymaps[setting_keys::keys::LT], GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_LeftThumbstick]).toULongLong();
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_RightThumbstick] = (WORD)settings->value(keymaps[setting_keys::keys::RT], GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_RightThumbstick]).toULongLong();
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_DPadDown] = (WORD)settings->value(keymaps[setting_keys::keys::DPADDOWN], GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_DPadDown]).toULongLong();
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_DPadUp] = (WORD)settings->value(keymaps[setting_keys::keys::DPADUP], GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_DPadUp]).toLongLong();
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_DPadRight] = (WORD)settings->value(keymaps[setting_keys::keys::DPADRIGHT], GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_DPadRight]).toLongLong();
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_DPadLeft] = (WORD)settings->value(keymaps[setting_keys::keys::DPADLEFT], GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_DPadLeft]).toLongLong();
}

/**
 * @brief load_all_settings
 * run the functions using the function array to load all the necessary settings.
 */
void load_all_settings() {
    for(int i=0;i<3;i++) {
        (*load_functions[i])();
    }
}


