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
#include <map>
#include <windows.h>

extern const std::map<WORD, const char *> vk_maps;
extern const QList<WORD> MOUSE_BUTTONS;
bool is_mouse_button(WORD vk);

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

	// Return reference to global GAMEPAD_BUTTONS
	std::map<GamepadButtons, ButtonInput> &gamepadButtons();
	void setGamepadButton(GamepadButtons btn, ButtonInput input);

	// Thumbstick settings
	std::map<Thumbstick, ThumbstickInput> &thumbstickInputs();
	void setThumbstickInput(Thumbstick thumbstick, ThumbstickInput input);

	void loadAll();
	void saveSetting(const QString &key, const QVariant &value);
	QVariant loadSetting(const QString &key);

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

  private:
	SettingsSingleton();
	~SettingsSingleton() override = default;
	SettingsSingleton(const SettingsSingleton &) = delete;
	SettingsSingleton &operator=(const SettingsSingleton &) = delete;

	QSettings settings;
	int mouse_sensitivity;
	int port_number;

	// KeyMaps
	std::map<GamepadButtons, ButtonInput> m_gamepadButtons;
	std::map<Thumbstick, ThumbstickInput> m_thumbstickInputs;

	QString m_activeProfileName;
	KeymapProfile m_activeKeymapProfile;

	void loadMouseSensitivity();
	void loadPort();
};
