#include "keymap_profile.hpp"

#include "../ui/buttoninputbox.hpp"
#include "settings.hpp"

#include <QDebug>
#include <QSettings>

#ifdef WIN32
#include <windows.h>
#elif defined(__linux__)
#include <linux/input.h>
#endif

void KeymapProfile::initializeDefaultMappings()
{
#ifdef WIN32
	buttonMappings = {{GamepadButtons::GamepadButtons_Menu, VK_MENU},
					  {GamepadButtons::GamepadButtons_View, VK_TAB},
					  {GamepadButtons::GamepadButtons_A, VK_RETURN},
					  {GamepadButtons::GamepadButtons_B, VK_ESCAPE},
					  {GamepadButtons::GamepadButtons_X, VK_SHIFT},
					  {GamepadButtons::GamepadButtons_Y, VK_CONTROL},
					  {GamepadButtons::GamepadButtons_DPadUp, VK_UP},
					  {GamepadButtons::GamepadButtons_DPadDown, VK_DOWN},
					  {GamepadButtons::GamepadButtons_DPadLeft, VK_LEFT},
					  {GamepadButtons::GamepadButtons_DPadRight, VK_RIGHT},
					  {GamepadButtons::GamepadButtons_LeftShoulder, VK_LBUTTON},
					  {GamepadButtons::GamepadButtons_RightShoulder, VK_RBUTTON}};

	// Initialize default display names for buttons
	buttonDisplayNames = {{GamepadButtons::GamepadButtons_Menu, "Menu"},
						  {GamepadButtons::GamepadButtons_View, "Tab"},
						  {GamepadButtons::GamepadButtons_A, "Enter"},
						  {GamepadButtons::GamepadButtons_B, "Escape"},
						  {GamepadButtons::GamepadButtons_X, "Shift"},
						  {GamepadButtons::GamepadButtons_Y, "Ctrl"},
						  {GamepadButtons::GamepadButtons_DPadUp, "Up"},
						  {GamepadButtons::GamepadButtons_DPadDown, "Down"},
						  {GamepadButtons::GamepadButtons_DPadLeft, "Left"},
						  {GamepadButtons::GamepadButtons_DPadRight, "Right"},
						  {GamepadButtons::GamepadButtons_LeftShoulder, "Left Click"},
						  {GamepadButtons::GamepadButtons_RightShoulder, "Right Click"}};

	thumbstickMappings = {
		{Thumbstick_Left,
		 {false, {'W', false, "W"}, {'S', false, "S"}, {'A', false, "A"}, {'D', false, "D"}}},
		{Thumbstick_Right,
		 {false,
		  {VK_UP, false, "Up"},
		  {VK_DOWN, false, "Down"},
		  {VK_LEFT, false, "Left"},
		  {VK_RIGHT, false, "Right"}}}};
#elif defined(__linux__)
	buttonMappings = {{GamepadButtons::GamepadButtons_Menu, KEY_MENU},
					  {GamepadButtons::GamepadButtons_View, KEY_TAB},
					  {GamepadButtons::GamepadButtons_A, KEY_ENTER},
					  {GamepadButtons::GamepadButtons_B, KEY_ESC},
					  {GamepadButtons::GamepadButtons_X, KEY_LEFTSHIFT},
					  {GamepadButtons::GamepadButtons_Y, KEY_LEFTCTRL},
					  {GamepadButtons::GamepadButtons_DPadUp, KEY_UP},
					  {GamepadButtons::GamepadButtons_DPadDown, KEY_DOWN},
					  {GamepadButtons::GamepadButtons_DPadLeft, KEY_LEFT},
					  {GamepadButtons::GamepadButtons_DPadRight, KEY_RIGHT},
					  {GamepadButtons::GamepadButtons_LeftShoulder, BTN_LEFT},
					  {GamepadButtons::GamepadButtons_RightShoulder, BTN_RIGHT}};

	// Initialize default display names for buttons
	buttonDisplayNames = {{GamepadButtons::GamepadButtons_Menu, "Menu"},
						  {GamepadButtons::GamepadButtons_View, "Tab"},
						  {GamepadButtons::GamepadButtons_A, "Enter"},
						  {GamepadButtons::GamepadButtons_B, "Escape"},
						  {GamepadButtons::GamepadButtons_X, "Left Shift"},
						  {GamepadButtons::GamepadButtons_Y, "Left Ctrl"},
						  {GamepadButtons::GamepadButtons_DPadUp, "Up"},
						  {GamepadButtons::GamepadButtons_DPadDown, "Down"},
						  {GamepadButtons::GamepadButtons_DPadLeft, "Left"},
						  {GamepadButtons::GamepadButtons_DPadRight, "Right"},
						  {GamepadButtons::GamepadButtons_LeftShoulder, "Left Click"},
						  {GamepadButtons::GamepadButtons_RightShoulder, "Right Click"}};

	thumbstickMappings = {{Thumbstick_Left,
						   {false,
							{KEY_W, false, "W"},
							{KEY_S, false, "S"},
							{KEY_A, false, "A"},
							{KEY_D, false, "D"}}},
						  {Thumbstick_Right,
						   {false,
							{KEY_UP, false, "Up"},
							{KEY_DOWN, false, "Down"},
							{KEY_LEFT, false, "Left"},
							{KEY_RIGHT, false, "Right"}}}};
#endif
}

bool KeymapProfile::load(const QString &profilePath) noexcept
{
	if (!QFile::exists(profilePath))
	{
		qInfo() << tr("Profile file %1 does not exist. Loading defaults.").arg(profilePath);
		initializeDefaultMappings();
		return true;
	}
	try
	{
		QSettings settings(profilePath, QSettings::IniFormat);
		loadFromSettings(settings);
	}
	catch (...)
	{
		qInfo()
			<< tr("Error loading keymap profile %1. Loading default mappings.").arg(profilePath);
		initializeDefaultMappings();
	}
	return true;
}

bool KeymapProfile::save(const QString &profilePath) const
{
	QSettings settings(profilePath, QSettings::IniFormat);
	saveToSettings(settings);
	settings.sync();
	return true;
}

InputKeyCode KeymapProfile::buttonMap(GamepadButtons btn) const
{
	auto it = buttonMappings.find(btn);
	return it != buttonMappings.end() ? it->second : 0;
}

void KeymapProfile::setButtonInput(GamepadButtons btn, InputKeyCode vk, const QString &displayName)
{
	buttonMappings[btn] = vk;
	buttonDisplayNames[btn] = displayName;
}

QString KeymapProfile::buttonDisplayName(GamepadButtons btn) const
{
	auto it = buttonDisplayNames.find(btn);
	return it != buttonDisplayNames.end() ? it->second : QString();
}

void KeymapProfile::setThumbstickInput(Thumbstick thumb, const ThumbstickInput &input)
{
	thumbstickMappings[thumb] = input;
}

ThumbstickInput KeymapProfile::thumbstickInput(Thumbstick thumb) const
{
	auto it = thumbstickMappings.find(thumb);
	return (it != thumbstickMappings.end()) ? it->second : ThumbstickInput{};
}

void KeymapProfile::setLeftThumbMouseMove(bool enabled)
{
	thumbstickMappings[Thumbstick_Left].is_mouse_move = enabled;
}

bool KeymapProfile::leftThumbMouseMove() const
{
	auto it = thumbstickMappings.find(Thumbstick_Left);
	return (it != thumbstickMappings.end()) ? it->second.is_mouse_move : false;
}

void KeymapProfile::setRightThumbMouseMove(bool enabled)
{
	thumbstickMappings[Thumbstick_Right].is_mouse_move = enabled;
}

bool KeymapProfile::rightThumbMouseMove() const
{
	auto it = thumbstickMappings.find(Thumbstick_Right);
	return (it != thumbstickMappings.end()) ? it->second.is_mouse_move : false;
}

void KeymapProfile::loadFromSettings(QSettings const &settings)
{
	qDebug() << "Loading button mappings from file:" << settings.fileName();

	// Clear existing mappings
	buttonMappings.clear();
	buttonDisplayNames.clear();

	// Load with platform-specific defaults
#ifdef WIN32
	buttonMappings[GamepadButtons_A] = settings.value("buttons/A", VK_RETURN).toUInt();
	buttonMappings[GamepadButtons_B] = settings.value("buttons/B", VK_ESCAPE).toUInt();
	buttonMappings[GamepadButtons_X] = settings.value("buttons/X", VK_SHIFT).toUInt();
	buttonMappings[GamepadButtons_Y] = settings.value("buttons/Y", VK_CONTROL).toUInt();
	buttonMappings[GamepadButtons_RightShoulder] =
		settings.value("buttons/RS", VK_RBUTTON).toUInt();
	buttonMappings[GamepadButtons_LeftShoulder] = settings.value("buttons/LS", VK_LBUTTON).toUInt();
	buttonMappings[GamepadButtons_DPadDown] = settings.value("buttons/DPADDOWN", VK_DOWN).toUInt();
	buttonMappings[GamepadButtons_DPadUp] = settings.value("buttons/DPADUP", VK_UP).toUInt();
	buttonMappings[GamepadButtons_DPadRight] =
		settings.value("buttons/DPADRIGHT", VK_RIGHT).toUInt();
	buttonMappings[GamepadButtons_DPadLeft] = settings.value("buttons/DPADLEFT", VK_LEFT).toUInt();
	buttonMappings[GamepadButtons_View] = settings.value("buttons/VIEW", VK_TAB).toUInt();
	buttonMappings[GamepadButtons_Menu] = settings.value("buttons/MENU", VK_APPS).toUInt();
#elif defined(__linux__)
	buttonMappings[GamepadButtons_A] = settings.value("buttons/A", KEY_ENTER).toUInt();
	buttonMappings[GamepadButtons_B] = settings.value("buttons/B", KEY_ESC).toUInt();
	buttonMappings[GamepadButtons_X] = settings.value("buttons/X", KEY_LEFTSHIFT).toUInt();
	buttonMappings[GamepadButtons_Y] = settings.value("buttons/Y", KEY_LEFTCTRL).toUInt();
	buttonMappings[GamepadButtons_RightShoulder] = settings.value("buttons/RS", BTN_RIGHT).toUInt();
	buttonMappings[GamepadButtons_LeftShoulder] = settings.value("buttons/LS", BTN_LEFT).toUInt();
	buttonMappings[GamepadButtons_DPadDown] = settings.value("buttons/DPADDOWN", KEY_DOWN).toUInt();
	buttonMappings[GamepadButtons_DPadUp] = settings.value("buttons/DPADUP", KEY_UP).toUInt();
	buttonMappings[GamepadButtons_DPadRight] =
		settings.value("buttons/DPADRIGHT", KEY_RIGHT).toUInt();
	buttonMappings[GamepadButtons_DPadLeft] = settings.value("buttons/DPADLEFT", KEY_LEFT).toUInt();
	buttonMappings[GamepadButtons_View] = settings.value("buttons/VIEW", KEY_TAB).toUInt();
	buttonMappings[GamepadButtons_Menu] = settings.value("buttons/MENU", KEY_MENU).toUInt();
#endif

	// Load display names
	buttonDisplayNames[GamepadButtons_A] = settings.value("button_display_names/A", "").toString();
	buttonDisplayNames[GamepadButtons_B] = settings.value("button_display_names/B", "").toString();
	buttonDisplayNames[GamepadButtons_X] = settings.value("button_display_names/X", "").toString();
	buttonDisplayNames[GamepadButtons_Y] = settings.value("button_display_names/Y", "").toString();
	buttonDisplayNames[GamepadButtons_RightShoulder] =
		settings.value("button_display_names/RS", "").toString();
	buttonDisplayNames[GamepadButtons_LeftShoulder] =
		settings.value("button_display_names/LS", "").toString();
	buttonDisplayNames[GamepadButtons_DPadDown] =
		settings.value("button_display_names/DPADDOWN", "").toString();
	buttonDisplayNames[GamepadButtons_DPadUp] =
		settings.value("button_display_names/DPADUP", "").toString();
	buttonDisplayNames[GamepadButtons_DPadRight] =
		settings.value("button_display_names/DPADRIGHT", "").toString();
	buttonDisplayNames[GamepadButtons_DPadLeft] =
		settings.value("button_display_names/DPADLEFT", "").toString();
	buttonDisplayNames[GamepadButtons_View] =
		settings.value("button_display_names/VIEW", "").toString();
	buttonDisplayNames[GamepadButtons_Menu] =
		settings.value("button_display_names/MENU", "").toString();

	// Log mappings for debugging
	qDebug() << "Loaded mappings:" << "A:" << buttonMappings[GamepadButtons_A]
			 << "B:" << buttonMappings[GamepadButtons_B] << "X:" << buttonMappings[GamepadButtons_X]
			 << "Y:" << buttonMappings[GamepadButtons_Y]
			 << "RS:" << buttonMappings[GamepadButtons_RightShoulder]
			 << "LS:" << buttonMappings[GamepadButtons_LeftShoulder]
			 << "DPADDOWN:" << buttonMappings[GamepadButtons_DPadDown]
			 << "DPADUP:" << buttonMappings[GamepadButtons_DPadUp]
			 << "DPADRIGHT:" << buttonMappings[GamepadButtons_DPadRight]
			 << "DPADLEFT:" << buttonMappings[GamepadButtons_DPadLeft]
			 << "VIEW:" << buttonMappings[GamepadButtons_View]
			 << "MENU:" << buttonMappings[GamepadButtons_Menu];

	// Thumbstick mappings
	ThumbstickInput left, right;
	left.is_mouse_move =
		settings.value(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstick], false)
			.toBool();
	left.up.vk =
		settings.value(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickUpKey], 'W')
			.toUInt();
	left.down.vk =
		settings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickDownKey], 'S')
			.toUInt();
	left.left.vk =
		settings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickLeftKey], 'A')
			.toUInt();
	left.right.vk =
		settings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickRightKey], 'D')
			.toUInt();

	right.is_mouse_move =
		settings.value(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstick], false)
			.toBool();
	right.up.vk =
		settings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickUpKey],
				   Qt::Key_Up)
			.toUInt();
	right.down.vk =
		settings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickDownKey],
				   Qt::Key_Down)
			.toUInt();
	right.left.vk =
		settings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickLeftKey],
				   Qt::Key_Left)
			.toUInt();
	right.right.vk =
		settings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickRightKey],
				   Qt::Key_Right)
			.toUInt();

	// Load thumbstick display names
	left.up.displayName =
		settings.value("thumbstick_display_names/LeftThumbstickUp", "").toString();
	left.down.displayName =
		settings.value("thumbstick_display_names/LeftThumbstickDown", "").toString();
	left.left.displayName =
		settings.value("thumbstick_display_names/LeftThumbstickLeft", "").toString();
	left.right.displayName =
		settings.value("thumbstick_display_names/LeftThumbstickRight", "").toString();
	right.up.displayName =
		settings.value("thumbstick_display_names/RightThumbstickUp", "").toString();
	right.down.displayName =
		settings.value("thumbstick_display_names/RightThumbstickDown", "").toString();
	right.left.displayName =
		settings.value("thumbstick_display_names/RightThumbstickLeft", "").toString();
	right.right.displayName =
		settings.value("thumbstick_display_names/RightThumbstickRight", "").toString();

	thumbstickMappings[Thumbstick_Left] = left;
	thumbstickMappings[Thumbstick_Right] = right;
}

void KeymapProfile::saveToSettings(QSettings &settings) const
{
	qDebug() << "Saving button mappings to file:";

	// Clear existing sections to ensure clean save
	settings.remove("buttons");
	settings.remove("button_display_names");
	settings.remove("thumbsticks");
	settings.remove("thumbstick_display_names");

	// Button mappings - Use explicit mapping to ensure correct values
	// Map GamepadButtons directly to settings keys
	settings.setValue("buttons/A", static_cast<uint>(buttonMap(GamepadButtons_A)));
	settings.setValue("buttons/B", static_cast<uint>(buttonMap(GamepadButtons_B)));
	settings.setValue("buttons/X", static_cast<uint>(buttonMap(GamepadButtons_X)));
	settings.setValue("buttons/Y", static_cast<uint>(buttonMap(GamepadButtons_Y)));
	settings.setValue("buttons/RS", static_cast<uint>(buttonMap(GamepadButtons_RightShoulder)));
	settings.setValue("buttons/LS", static_cast<uint>(buttonMap(GamepadButtons_LeftShoulder)));
	settings.setValue("buttons/DPADDOWN", static_cast<uint>(buttonMap(GamepadButtons_DPadDown)));
	settings.setValue("buttons/DPADUP", static_cast<uint>(buttonMap(GamepadButtons_DPadUp)));
	settings.setValue("buttons/DPADRIGHT", static_cast<uint>(buttonMap(GamepadButtons_DPadRight)));
	settings.setValue("buttons/DPADLEFT", static_cast<uint>(buttonMap(GamepadButtons_DPadLeft)));
	settings.setValue("buttons/VIEW", static_cast<uint>(buttonMap(GamepadButtons_View)));
	settings.setValue("buttons/MENU", static_cast<uint>(buttonMap(GamepadButtons_Menu)));

	// Button display names
	settings.setValue("button_display_names/A", buttonDisplayName(GamepadButtons_A));
	settings.setValue("button_display_names/B", buttonDisplayName(GamepadButtons_B));
	settings.setValue("button_display_names/X", buttonDisplayName(GamepadButtons_X));
	settings.setValue("button_display_names/Y", buttonDisplayName(GamepadButtons_Y));
	settings.setValue("button_display_names/RS", buttonDisplayName(GamepadButtons_RightShoulder));
	settings.setValue("button_display_names/LS", buttonDisplayName(GamepadButtons_LeftShoulder));
	settings.setValue("button_display_names/DPADDOWN", buttonDisplayName(GamepadButtons_DPadDown));
	settings.setValue("button_display_names/DPADUP", buttonDisplayName(GamepadButtons_DPadUp));
	settings.setValue("button_display_names/DPADRIGHT",
					  buttonDisplayName(GamepadButtons_DPadRight));
	settings.setValue("button_display_names/DPADLEFT", buttonDisplayName(GamepadButtons_DPadLeft));
	settings.setValue("button_display_names/VIEW", buttonDisplayName(GamepadButtons_View));
	settings.setValue("button_display_names/MENU", buttonDisplayName(GamepadButtons_Menu));

	// Log mapping values for debugging
	qDebug() << "A:" << buttonMap(GamepadButtons_A) << "B:" << buttonMap(GamepadButtons_B)
			 << "X:" << buttonMap(GamepadButtons_X) << "Y:" << buttonMap(GamepadButtons_Y)
			 << "RS:" << buttonMap(GamepadButtons_RightShoulder)
			 << "LS:" << buttonMap(GamepadButtons_LeftShoulder)
			 << "DPADDOWN:" << buttonMap(GamepadButtons_DPadDown)
			 << "DPADUP:" << buttonMap(GamepadButtons_DPadUp)
			 << "DPADRIGHT:" << buttonMap(GamepadButtons_DPadRight)
			 << "DPADLEFT:" << buttonMap(GamepadButtons_DPadLeft)
			 << "VIEW:" << buttonMap(GamepadButtons_View)
			 << "MENU:" << buttonMap(GamepadButtons_Menu);

	// Thumbstick mappings - keep unchanged
	const auto &left = thumbstickMappings.at(Thumbstick_Left);
	const auto &right = thumbstickMappings.at(Thumbstick_Right);

	settings.setValue(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstick],
					  left.is_mouse_move);
	settings.setValue(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickUpKey],
					  left.up.vk);
	settings.setValue(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickDownKey],
					  left.down.vk);
	settings.setValue(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickLeftKey],
					  left.left.vk);
	settings.setValue(thumbstick_settings[setting_keys::thumbstick_keys::LeftThumbstickRightKey],
					  left.right.vk);

	settings.setValue(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstick],
					  right.is_mouse_move);
	settings.setValue(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickUpKey],
					  right.up.vk);
	settings.setValue(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickDownKey],
					  right.down.vk);
	settings.setValue(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickLeftKey],
					  right.left.vk);
	settings.setValue(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickRightKey],
					  right.right.vk);

	// Thumbstick display names
	settings.setValue("thumbstick_display_names/LeftThumbstickUp", left.up.displayName);
	settings.setValue("thumbstick_display_names/LeftThumbstickDown", left.down.displayName);
	settings.setValue("thumbstick_display_names/LeftThumbstickLeft", left.left.displayName);
	settings.setValue("thumbstick_display_names/LeftThumbstickRight", left.right.displayName);
	settings.setValue("thumbstick_display_names/RightThumbstickUp", right.up.displayName);
	settings.setValue("thumbstick_display_names/RightThumbstickDown", right.down.displayName);
	settings.setValue("thumbstick_display_names/RightThumbstickLeft", right.left.displayName);
	settings.setValue("thumbstick_display_names/RightThumbstickRight", right.right.displayName);
}

ButtonInput KeymapProfile::buttonInput(GamepadButtons button) const
{
	ButtonInput input;
	auto it = buttonMappings.find(button);
	if (it != buttonMappings.end())
	{
		input.vk = it->second;
		input.is_mouse_button = is_mouse_button(input.vk);
		input.displayName = buttonDisplayName(button);
	}
	return input;
}
