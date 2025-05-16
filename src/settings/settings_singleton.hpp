#ifndef SETTINGS_SINGLETON_H
#define SETTINGS_SINGLETON_H

#include "input_types.hpp"
#include "settings_key_variables.hpp"

#include <QDir>
#include <QList>
#include <QMap>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QVariant>
#include <map>

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

  private:
	SettingsSingleton();
	~SettingsSingleton() override = default;
	SettingsSingleton(const SettingsSingleton &) = delete;
	SettingsSingleton &operator=(const SettingsSingleton &) = delete;

	QSettings settings;
	int mouse_sensitivity;
	int port_number;

	void loadMouseSensitivity();
	void loadPort();
	void loadKeyMaps();
	void loadThumbstickMaps();
};

#endif // SETTINGS_SINGLETON_H
