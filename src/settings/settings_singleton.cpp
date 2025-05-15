#include "settings_singleton.hpp"
#include "settings.hpp"
#include "settings_key_variables.hpp"
#include <QDebug>

SettingsSingleton::SettingsSingleton()
	: settings(QDir::toNativeSeparators(QDir::homePath() + "/VirtualGamePad.ini"),
			   QSettings::IniFormat)
{
	loadAll();
}

void SettingsSingleton::setMouseSensitivity(int value)
{
	mouse_sensitivity = value;
	saveSetting(setting_keys::Mouse_sensitivity, mouse_sensitivity / 100);
}

void SettingsSingleton::setPort(int value)
{
	port_number = value;
	saveSetting(server_settings[setting_keys::Port], port_number);
}

std::map<GamepadButtons, Input> &SettingsSingleton::gamepadButtons()
{
	return GAMEPAD_BUTTONS;
}

void SettingsSingleton::setGamepadButton(GamepadButtons btn, Input input)
{
	GAMEPAD_BUTTONS[btn] = input;
	// Save logic can be added if needed
}

void SettingsSingleton::saveSetting(const QString &key, const QVariant &value)
{
	settings.setValue(key, value);
	settings.sync();
}

QVariant SettingsSingleton::loadSetting(const QString &key)
{
	return settings.value(key);
}

void SettingsSingleton::loadMouseSensitivity()
{
	mouse_sensitivity = settings.value(setting_keys::Mouse_sensitivity, 10).toInt() * 100;
}

void SettingsSingleton::loadPort()
{
	port_number = settings.value(server_settings[setting_keys::Port], 7878).toInt();
}

void SettingsSingleton::loadKeyMaps()
{
	// This should be similar to the previous load_key_maps logic
	// ...
}

void SettingsSingleton::loadAll()
{
	loadMouseSensitivity();
	loadPort();
	loadKeyMaps();
}
