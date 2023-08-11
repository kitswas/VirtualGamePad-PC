#include "settings.h"
#include "settings_key_variables.h"
#include <QDebug>

QString SETTINGS_FILE = QDir::toNativeSeparators(QDir::homePath() + "//VirtualGamePad.Ini");
QSettings *settings;

QString setting_keys::Mouse_sensivity = "mouse_setting/mouse_sensivity";

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

void load_mouse_setting()
{
	mouse_sensivity = settings->value(setting_keys::Mouse_sensivity).toInt() * 100;
}

void load_settings_file(QObject *parent = nullptr)
{
	settings = new QSettings(SETTINGS_FILE, QSettings::Format::IniFormat, parent);
}

void set_port_number(int custom_port) {
    settings->setValue(server_settings[setting_keys::Port], custom_port);
}

void load_port_number() {
    port = settings->value(server_settings[setting_keys::Port]).toInt();
}

void set_key_maps(setting_keys::keys a,WORD value) {
    settings->setValue(keymaps[a], value);
}

void load_key_maps() {
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_A] = (WORD)settings->value(keymaps[setting_keys::keys::A]).toULongLong();
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_B] = (WORD)settings->value(keymaps[setting_keys::keys::B]).toULongLong();
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_X] = (WORD)settings->value(keymaps[setting_keys::keys::X]).toULongLong();
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_Y] = (WORD)settings->value(keymaps[setting_keys::keys::Y]).toULongLong();
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_LeftThumbstick] = (WORD)settings->value(keymaps[setting_keys::keys::LT]).toULongLong();
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_RightThumbstick] = (WORD)settings->value(keymaps[setting_keys::keys::RT]).toULongLong();
}


