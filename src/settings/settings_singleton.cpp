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

	// Find the corresponding setting key for this button
	for (auto it = keymaps.begin(); it != keymaps.end(); ++it)
	{
		GamepadButtons mapped_btn;
		setting_keys::keys key = it.key();

		switch (key)
		{
		case setting_keys::keys::A:
			mapped_btn = GamepadButtons_A;
			break;
		case setting_keys::keys::B:
			mapped_btn = GamepadButtons_B;
			break;
		case setting_keys::keys::X:
			mapped_btn = GamepadButtons_X;
			break;
		case setting_keys::keys::Y:
			mapped_btn = GamepadButtons_Y;
			break;
		case setting_keys::keys::LSHDR:
			mapped_btn = GamepadButtons_LeftShoulder;
			break;
		case setting_keys::keys::RSHDR:
			mapped_btn = GamepadButtons_RightShoulder;
			break;
		case setting_keys::keys::DPADDOWN:
			mapped_btn = GamepadButtons_DPadDown;
			break;
		case setting_keys::keys::DPADUP:
			mapped_btn = GamepadButtons_DPadUp;
			break;
		case setting_keys::keys::DPADLEFT:
			mapped_btn = GamepadButtons_DPadLeft;
			break;
		case setting_keys::keys::DPADRIGHT:
			mapped_btn = GamepadButtons_DPadRight;
			break;
		case setting_keys::keys::MENU:
			mapped_btn = GamepadButtons_Menu;
			break;
		case setting_keys::keys::VIEW:
			mapped_btn = GamepadButtons_View;
			break;
		default:
			continue;
		}

		if (mapped_btn == btn)
		{
			saveSetting(it.value(), input.vk);
			break;
		}
	}
}

std::map<Thumbstick, ThumbstickInput> &SettingsSingleton::thumbstickInputs()
{
	return m_thumbstickInputs;
}

void SettingsSingleton::setThumbstickInput(Thumbstick thumbstick, ThumbstickInput input)
{
	m_thumbstickInputs[thumbstick] = input;

	// Save the thumbstick mapping to settings
	switch (thumbstick)
	{
	case Thumbstick_Left:
		saveSetting(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstick],
					input.is_mouse_move ? 1 : 0);
		saveSetting(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickUpKey],
					input.up.vk);
		saveSetting(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickDownKey],
					input.down.vk);
		saveSetting(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickLeftKey],
					input.left.vk);
		saveSetting(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickRightKey],
					input.right.vk);
		break;
	case Thumbstick_Right:
		saveSetting(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstick],
					input.is_mouse_move ? 1 : 0);
		saveSetting(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickUpKey],
					input.up.vk);
		saveSetting(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickDownKey],
					input.down.vk);
		saveSetting(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickLeftKey],
					input.left.vk);
		saveSetting(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickRightKey],
					input.right.vk);
		break;
	}
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
	// Load button mappings
	for (auto it = keymaps.begin(); it != keymaps.end(); ++it)
	{
		setting_keys::keys key = it.key();
		QString settingKey = it.value();

		int vk = settings.value(settingKey, -1).toInt();
		if (vk != -1)
		{
			GamepadButtons btn;
			switch (key)
			{
			case setting_keys::keys::A:
				btn = GamepadButtons_A;
				break;
			case setting_keys::keys::B:
				btn = GamepadButtons_B;
				break;
			case setting_keys::keys::X:
				btn = GamepadButtons_X;
				break;
			case setting_keys::keys::Y:
				btn = GamepadButtons_Y;
				break;
			case setting_keys::keys::LSHDR:
				btn = GamepadButtons_LeftShoulder;
				break;
			case setting_keys::keys::RSHDR:
				btn = GamepadButtons_RightShoulder;
				break;
			case setting_keys::keys::DPADDOWN:
				btn = GamepadButtons_DPadDown;
				break;
			case setting_keys::keys::DPADUP:
				btn = GamepadButtons_DPadUp;
				break;
			case setting_keys::keys::DPADLEFT:
				btn = GamepadButtons_DPadLeft;
				break;
			case setting_keys::keys::DPADRIGHT:
				btn = GamepadButtons_DPadRight;
				break;
			case setting_keys::keys::MENU:
				btn = GamepadButtons_Menu;
				break;
			case setting_keys::keys::VIEW:
				btn = GamepadButtons_View;
				break;
			default:
				continue;
			}
			m_gamepadButtons[btn].vk = vk;
			m_gamepadButtons[btn].is_mouse_button = is_mouse_button(vk);
		}
	}
}

void SettingsSingleton::loadThumbstickMaps()
{
	// Load left thumbstick settings
	ThumbstickInput leftInput;
	leftInput.is_mouse_move =
		settings.value(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstick], false)
			.toBool();
	leftInput.up.vk =
		settings.value(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickUpKey], 'W')
			.toInt();
	leftInput.up.is_mouse_button = is_mouse_button(leftInput.up.vk);
	leftInput.down.vk =
		settings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickDownKey], 'S')
			.toInt();
	leftInput.down.is_mouse_button = is_mouse_button(leftInput.down.vk);
	leftInput.left.vk =
		settings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickLeftKey], 'A')
			.toInt();
	leftInput.left.is_mouse_button = is_mouse_button(leftInput.left.vk);
	leftInput.right.vk =
		settings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickRightKey], 'D')
			.toInt();
	leftInput.right.is_mouse_button = is_mouse_button(leftInput.right.vk);

	m_thumbstickInputs[Thumbstick_Left] = leftInput;

	// Load right thumbstick settings
	ThumbstickInput rightInput;
	rightInput.is_mouse_move =
		settings.value(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstick], true)
			.toBool();
	rightInput.up.vk =
		settings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickUpKey], VK_UP)
			.toInt();
	rightInput.up.is_mouse_button = is_mouse_button(rightInput.up.vk);
	rightInput.down.vk =
		settings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickDownKey],
				   VK_DOWN)
			.toInt();
	rightInput.down.is_mouse_button = is_mouse_button(rightInput.down.vk);
	rightInput.left.vk =
		settings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickLeftKey],
				   VK_LEFT)
			.toInt();
	rightInput.left.is_mouse_button = is_mouse_button(rightInput.left.vk);
	rightInput.right.vk =
		settings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickRightKey],
				   VK_RIGHT)
			.toInt();
	rightInput.right.is_mouse_button = is_mouse_button(rightInput.right.vk);

	m_thumbstickInputs[Thumbstick_Right] = rightInput;
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
