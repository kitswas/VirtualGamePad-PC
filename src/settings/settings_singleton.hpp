#ifndef SETTINGS_SINGLETON_H
#define SETTINGS_SINGLETON_H

#include "../../VGP_Data_Exchange/C/GameButtons.h"
#include "input_types.hpp"

#include <QDir>
#include <QList>
#include <QMap>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QVariant>
#include <map>
#include <windows.h>

extern std::map<WORD, const char *> vk_maps;
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

	void loadMouseSensitivity();
	void loadPort();
};

#endif // SETTINGS_SINGLETON_H
