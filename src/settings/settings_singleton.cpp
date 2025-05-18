#include "settings_singleton.hpp"

#include "settings.hpp"

#include <QApplication>
#include <QDebug>

const std::map<WORD, const char *> vk_maps = {{VK_LBUTTON, "LMButton"},
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
	: settings(QDir::toNativeSeparators(QApplication::applicationDirPath() + "/VirtualGamePad.ini"),
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
	// Initialize active keymap profile based on saved profile name
	m_activeProfileName = activeProfile;
	{
		QString profilePath = QDir::toNativeSeparators(QApplication::applicationDirPath() +
													   "/profiles/" + m_activeProfileName + ".ini");
		m_activeKeymapProfile.load(profilePath);
	}
}

void SettingsSingleton::setMouseSensitivity(int value)
{
	mouse_sensitivity = value;
	saveSetting(setting_keys::Mouse_sensitivity, mouse_sensitivity / MOUSE_SENSITIVITY_MULTIPLIER);
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
	mouse_sensitivity =
		MOUSE_SENSITIVITY_MULTIPLIER *
		settings.value(setting_keys::Mouse_sensitivity, DEFAULT_MOUSE_SENSITIVITY).toInt();
}

void SettingsSingleton::loadPort()
{
	port_number = settings.value(server_settings[setting_keys::Port], DEFAULT_PORT_NUMBER).toInt();
}

void SettingsSingleton::loadAll()
{
	try
	{
		loadMouseSensitivity();
		loadPort();
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

// Active keymap profile accessors
QString SettingsSingleton::activeProfileName() const
{
	return m_activeProfileName;
}

void SettingsSingleton::setActiveProfileName(const QString &name)
{
	m_activeProfileName = name;
	settings.setValue("profiles/active", m_activeProfileName);
	settings.sync();

	// Don't reload the profile here - it causes double loading issues
	// The caller should already have loaded the profile
}

KeymapProfile &SettingsSingleton::activeKeymapProfile()
{
	return m_activeKeymapProfile;
}

// Profile management methods
QString SettingsSingleton::getProfilesDir() const
{
	return QDir::toNativeSeparators(QApplication::applicationDirPath() + "/profiles");
}

QStringList SettingsSingleton::listAvailableProfiles() const
{
	QStringList profiles;
	QDir dir(getProfilesDir());
	if (!dir.exists())
	{
		dir.mkpath(".");
		return QStringList{"Default"};
	}

	QStringList nameFilters;
	nameFilters << "*.ini";
	QFileInfoList fileList = dir.entryInfoList(nameFilters, QDir::Files, QDir::Name);

	foreach (const QFileInfo &fileInfo, fileList)
	{
		profiles.append(fileInfo.baseName());
	}

	// If no profiles exist, add a default one
	if (profiles.isEmpty())
	{
		profiles.append("Default");
	}

	return profiles;
}

bool SettingsSingleton::createProfile(const QString &profileName)
{
	if (profileName.isEmpty())
		return false;

	// Check if profile already exists
	if (profileExists(profileName))
		return false;

	// Create directory if needed
	QDir dir(getProfilesDir());
	if (!dir.exists())
		dir.mkpath(".");

	// Save current mappings to this profile
	QString profilePath = getProfilesDir() + "/" + profileName + ".ini";
	bool success = m_activeKeymapProfile.save(profilePath);

	if (success)
		setActiveProfileName(profileName);

	return success;
}

bool SettingsSingleton::deleteProfile(const QString &profileName)
{
	if (profileName.isEmpty())
		return false;

	// Don't delete active profile
	if (profileName == m_activeProfileName)
		return false;

	QString profilePath = getProfilesDir() + "/" + profileName + ".ini";
	QFile file(profilePath);
	return file.remove();
}

bool SettingsSingleton::profileExists(const QString &profileName) const
{
	if (profileName.isEmpty())
		return false;

	QDir dir(getProfilesDir());
	return QFile::exists(dir.filePath(profileName + ".ini"));
}

bool SettingsSingleton::loadProfile(const QString &profileName)
{
	if (profileName.isEmpty())
		return false;

	// Create standard profile path
	QDir dir(getProfilesDir());
	if (!dir.exists())
	{
		dir.mkpath(".");
	}

	QString profilePath = getProfilesDir() + "/" + profileName + ".ini";

	if (!QFile::exists(profilePath))
	{
		qInfo() << "Creating new profile at:" << profilePath;

		// Use default profile as template, or create new if none exists
		if (m_activeKeymapProfile.buttonMappings.empty())
		{
			m_activeKeymapProfile.initializeDefaultMappings();
		}

		if (!m_activeKeymapProfile.save(profilePath))
		{
			qWarning() << "Failed to save new profile at:" << profilePath;
			return false;
		}

		qInfo() << "New profile created successfully at:" << profilePath;
	}

	// Load the profile
	bool success = m_activeKeymapProfile.load(profilePath);
	if (success)
	{
		// Don't reload the profile when setting name - this would cause a double load
		m_activeProfileName = profileName;
		settings.setValue("profiles/active", m_activeProfileName);
		settings.sync();

		qInfo() << "Successfully loaded profile:" << profileName << "from" << profilePath;
	}
	else
	{
		qWarning() << "Failed to load profile:" << profileName << "from" << profilePath;
	}

	return success;
}

bool SettingsSingleton::saveActiveProfile()
{
	if (m_activeProfileName.isEmpty())
		return false;

	// Ensure the profiles directory exists
	QDir dir(getProfilesDir());
	if (!dir.exists())
	{
		dir.mkpath(".");
	}

	QString profilePath = getProfilesDir() + "/" + m_activeProfileName + ".ini";
	bool success = m_activeKeymapProfile.save(profilePath);

	if (success)
	{
		qDebug() << "Successfully saved profile:" << m_activeProfileName << "to" << profilePath;
	}
	else
	{
		qWarning() << "Failed to save profile:" << m_activeProfileName << "to" << profilePath;
	}

	return success;
}
