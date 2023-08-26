#include "settings.h"
#include "settings_key_variables.h"
#include <QDebug>

QString SETTINGS_FILE = QDir::toNativeSeparators(QDir::homePath() + "//VirtualGamePad.Ini");
QSettings *settings;

QString setting_keys::Mouse_sensivity = "mouse_setting/mouse_sensivity";

void (*load_functions[3])(void) = {load_mouse_setting, load_port_number, load_key_maps};

void save_setting(QString key, QVariant value)
{
	settings->setValue(key, value);
	settings->sync();
	qDebug() << settings->value(key).toString() << Qt::endl;
	qDebug() << settings->fileName() << Qt::endl;
}

QVariant load_setting(QString key)
{
	QVariant value = settings->value(key);
	return value;
}

void set_mouse_sensivity(int sensivity) {
    mouse_sensivity = sensivity;
}


void load_mouse_setting()
{
    mouse_sensivity = settings->value(setting_keys::Mouse_sensivity).toInt() * 100;
}

void load_settings_file(QObject *parent = nullptr)
{
	settings = new QSettings(SETTINGS_FILE, QSettings::Format::IniFormat, parent);
}

void load_port_number() // set the port number as user_defined
{
    port = settings->value(server_settings[setting_keys::Port]).toInt();
}

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

void load_all_settings() {
    for(int i=0;i<3;i++) {
        (*load_functions[i])();
    }
}


