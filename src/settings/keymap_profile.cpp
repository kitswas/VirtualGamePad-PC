#include "keymap_profile.hpp"

#include "settings.hpp"

#include <QDebug>
#include <QFile>

void KeymapProfile::initializeDefaultMappings()
{
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

	thumbstickMappings = {
		{Thumbstick_Left, {false, {'W', false}, {'S', false}, {'A', false}, {'D', false}}},
		{Thumbstick_Right,
		 {false, {VK_UP, false}, {VK_DOWN, false}, {VK_LEFT, false}, {VK_RIGHT, false}}}};
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

void KeymapProfile::setButtonMap(GamepadButtons btn, WORD vk)
{
	buttonMappings[btn] = vk;
}

WORD KeymapProfile::buttonMap(GamepadButtons btn) const
{
	auto it = buttonMappings.find(btn);
	return (it != buttonMappings.end()) ? it->second : 0;
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

	// Explicitly load each button to avoid enum mapping issues
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
	buttonMappings[GamepadButtons_Menu] = settings.value("buttons/MENU", VK_MENU).toUInt();

	// Log mappings for debugging
	qDebug() << "Loaded mappings:"
			 << "A:" << buttonMappings[GamepadButtons_A] << "B:" << buttonMappings[GamepadButtons_B]
			 << "X:" << buttonMappings[GamepadButtons_X] << "Y:" << buttonMappings[GamepadButtons_Y]
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
			.value(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickUpKey], VK_UP)
			.toUInt();
	right.down.vk =
		settings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickDownKey],
				   VK_DOWN)
			.toUInt();
	right.left.vk =
		settings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickLeftKey],
				   VK_LEFT)
			.toUInt();
	right.right.vk =
		settings
			.value(thumbstick_settings[setting_keys::thumbstick_keys::RightThumbstickRightKey],
				   VK_RIGHT)
			.toUInt();

	thumbstickMappings[Thumbstick_Left] = left;
	thumbstickMappings[Thumbstick_Right] = right;
}

void KeymapProfile::saveToSettings(QSettings &settings) const
{
	qDebug() << "Saving button mappings to file:";

	// Clear existing sections to ensure clean save
	settings.remove("buttons");
	settings.remove("thumbsticks");

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
}
