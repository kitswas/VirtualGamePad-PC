#pragma once

#include "../../VGP_Data_Exchange/C/GameButtons.h"
#include "input_types.hpp"
#include "keymap_profile.hpp"

#include <QDir>
#include <QList>
#include <QMap>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QVariant>

enum class ExecutorType
{
	GamepadExecutor,
	KeyboardMouseExecutor
};

class SettingsSingleton : public QObject
{
	Q_OBJECT
  public:
	static SettingsSingleton &instance()
	{
		static SettingsSingleton _instance;
		return _instance;
	}

	QSettings *qsettings()
	{
		return &settings;
	}

	int mouseSensitivity() const
	{
		return mouse_sensitivity;
	}
	void setMouseSensitivity(int value);

	int port() const
	{
		return port_number;
	}
	void setPort(int value);

	ExecutorType executorType() const
	{
		return executor_type;
	}
	void setExecutorType(ExecutorType type);

	void loadAll();
	void saveSetting(const QString &key, const QVariant &value);
	QVariant loadSetting(const QString &key);

	void resetToDefaults();

	QString activeProfileName() const;
	void setActiveProfileName(const QString &name);
	KeymapProfile &activeKeymapProfile();

	// Profile management methods
	QString getProfilesDir() const;
	QStringList listAvailableProfiles() const;
	bool createProfile(const QString &profileName);
	bool deleteProfile(const QString &profileName);
	bool profileExists(const QString &profileName) const;
	bool loadProfile(const QString &profileName);
	bool saveActiveProfile();

	static constexpr int DEFAULT_MOUSE_SENSITIVITY = 10;
	static constexpr int MOUSE_SENSITIVITY_MULTIPLIER = 100;
	static constexpr int DEFAULT_PORT_NUMBER = 0;
	static constexpr ExecutorType DEFAULT_EXECUTOR_TYPE = ExecutorType::KeyboardMouseExecutor;

  private:
	SettingsSingleton();
	~SettingsSingleton() override = default;
	SettingsSingleton(const SettingsSingleton &) = delete;
	SettingsSingleton &operator=(const SettingsSingleton &) = delete;

	QSettings settings;
	int mouse_sensitivity;
	int port_number;
	ExecutorType executor_type;

	QString m_activeProfileName;
	KeymapProfile m_activeKeymapProfile;

	void loadMouseSensitivity();
	void loadPort();
	void loadExecutorType();
};
