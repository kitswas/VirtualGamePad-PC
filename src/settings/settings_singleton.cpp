#include "settings_singleton.hpp"

#include "settings.hpp"

#include <QApplication>
#include <QDebug>

SettingsSingleton::SettingsSingleton()
	: settings(QDir::toNativeSeparators(QApplication::applicationDirPath() + "/VirtualGamePad.ini"),
			   QSettings::IniFormat),
	  executor_type(DEFAULT_EXECUTOR_TYPE)
{
	qInfo() << "Settings file path:" << settings.fileName();

	loadAll();

	QString activeProfile = settings.value("profiles/active", "Default").toString();

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

void SettingsSingleton::setExecutorType(ExecutorType type)
{
	executor_type = type;
	saveSetting(setting_keys::Executor_type, static_cast<int>(executor_type));
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

void SettingsSingleton::loadExecutorType()
{
	executor_type = static_cast<ExecutorType>(
		settings.value(setting_keys::Executor_type, static_cast<int>(DEFAULT_EXECUTOR_TYPE))
			.toInt());
}

void SettingsSingleton::loadAll()
{
	try
	{
		loadMouseSensitivity();
		loadPort();
		loadExecutorType();
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
	return QApplication::applicationDirPath() + "/profiles";
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

void SettingsSingleton::resetToDefaults()
{
	// Reset mouse sensitivity
	setMouseSensitivity(DEFAULT_MOUSE_SENSITIVITY * MOUSE_SENSITIVITY_MULTIPLIER);

	// Reset port number
	setPort(DEFAULT_PORT_NUMBER);

	// Reset keymapping in active profile to defaults
	m_activeKeymapProfile.initializeDefaultMappings();

	// Do not save anything till the user confirms in the UI
	qInfo() << "Settings reset to defaults in UI.";
}
