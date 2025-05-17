#include "settings_singleton.hpp"

#include "settings.hpp"

#include <QApplication>
#include <QDebug>

std::map<WORD, const char *> vk_maps = {{VK_LBUTTON, "LMButton"},
										{VK_RBUTTON, "RMButton"},
										{VK_MBUTTON, "MMButton"},
										{VK_BACK, "BACKSPACE"},
										{VK_TAB, "TAB"},
										{VK_RETURN, "ENTER"},
										{VK_SHIFT, "SHIFT"},
										{VK_CONTROL, "CTRL"},
										{VK_CAPITAL, "CAPITAL"},
										{VK_ESCAPE, "ESCAPE"},
										{VK_SPACE, "SPACE"},
										{VK_PRIOR, "PageUP"},
										{VK_NEXT, "PageDOWN"},
										{VK_END, "END"},
										{VK_HOME, "HOME"},
										{VK_LEFT, "LEFT"},
										{VK_UP, "UP"},
										{VK_RIGHT, "RIGHT"},
										{VK_DOWN, "DOWN"},
										{VK_INSERT, "INS"},
										{VK_DELETE, "DEL"},
										{VK_OEM_PERIOD, "."},
										{VK_OEM_COMMA, ","},
										{VK_OEM_MINUS, "-"},
										{VK_OEM_PLUS, "+"},
										{VK_MENU, "MENU"}};

const QList<WORD> MOUSE_BUTTONS = {VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_XBUTTON1, VK_XBUTTON2};

bool is_mouse_button(WORD vk)
{
	return MOUSE_BUTTONS.contains(vk);
}

SettingsSingleton::SettingsSingleton()
	: settings(QDir::toNativeSeparators(qApp->applicationDirPath() + "/VirtualGamePad.ini"),
			   QSettings::IniFormat)
{
	qInfo() << "Settings file path:" << settings.fileName();

	// Optionally: Load active profile name for use elsewhere
	QString activeProfile = settings.value("profiles/active", "Default").toString();

	// Initialize default mappings
	m_gamepadButtons = {
		{GamepadButtons::GamepadButtons_Menu, ButtonInput{VK_MENU, false}},
		{GamepadButtons::GamepadButtons_View, ButtonInput{VK_TAB, false}},
		{GamepadButtons::GamepadButtons_A, ButtonInput{VK_RETURN, false}},
		{GamepadButtons::GamepadButtons_B, ButtonInput{VK_ESCAPE, false}},
		{GamepadButtons::GamepadButtons_X, ButtonInput{VK_SHIFT, false}},
		{GamepadButtons::GamepadButtons_Y, ButtonInput{VK_CONTROL, false}},
		{GamepadButtons::GamepadButtons_DPadUp, ButtonInput{VK_UP, false}},
		{GamepadButtons::GamepadButtons_DPadDown, ButtonInput{VK_DOWN, false}},
		{GamepadButtons::GamepadButtons_DPadLeft, ButtonInput{VK_LEFT, false}},
		{GamepadButtons::GamepadButtons_DPadRight, ButtonInput{VK_RIGHT, false}},
		{GamepadButtons::GamepadButtons_LeftShoulder, ButtonInput{VK_LBUTTON, true}},
		{GamepadButtons::GamepadButtons_RightShoulder, ButtonInput{VK_RBUTTON, true}}};

	m_thumbstickInputs = {
		{Thumbstick_Left, {false, {'W', false}, {'S', false}, {'A', false}, {'D', false}}},
		{Thumbstick_Right,
		 {false, {VK_UP, false}, {VK_DOWN, false}, {VK_LEFT, false}, {VK_RIGHT, false}}}};

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

std::map<GamepadButtons, ButtonInput> &SettingsSingleton::gamepadButtons()
{
	return m_gamepadButtons;
}

void SettingsSingleton::setGamepadButton(GamepadButtons btn, ButtonInput input)
{
	m_gamepadButtons[btn] = input;

	// Do NOT save to VirtualGamePad.ini here.
	// Keymap saving is now handled by the profile system only.
}

std::map<Thumbstick, ThumbstickInput> &SettingsSingleton::thumbstickInputs()
{
	return m_thumbstickInputs;
}

void SettingsSingleton::setThumbstickInput(Thumbstick thumbstick, ThumbstickInput input)
{
	m_thumbstickInputs[thumbstick] = input;

	// Do NOT save to VirtualGamePad.ini here.
	// Thumbstick mapping saving is now handled by the profile system only.
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
	// Do NOT load keymaps from VirtualGamePad.ini.
	// Keymaps are loaded from the active profile only.
}

void SettingsSingleton::loadThumbstickMaps()
{
	// Do NOT load thumbstick maps from VirtualGamePad.ini.
	// Thumbstick maps are loaded from the active profile only.
}

void SettingsSingleton::loadAll()
{
	try
	{
		loadMouseSensitivity();
		loadPort();
		loadKeyMaps();
		loadThumbstickMaps();
	}
	catch (const std::exception &e)
	{
		qDebug() << "Error loading settings:" << e.what();
		qInfo() << "Loading default settings";
		settings.clear();
		settings.sync();
	}
	catch (...)
	{
		qDebug() << "Unknown error loading settings";
	}
}
