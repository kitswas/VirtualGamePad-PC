#pragma once

#include "input_types.hpp"

#include <QFile>
#include <QObject>
#include <QSettings>
#include <QString>
#include <map>

class KeymapProfile : public QObject
{
	Q_OBJECT

  public:
	KeymapProfile() = default;
	~KeymapProfile() final = default;
	KeymapProfile(const KeymapProfile &) = default;
	KeymapProfile &operator=(const KeymapProfile &) = default;

	bool load(const QString &profilePath) noexcept;
	bool save(const QString &profilePath) const;

	void setButtonMap(GamepadButtons btn, InputKeyCode vk);
	InputKeyCode buttonMap(GamepadButtons btn) const;

	void setThumbstickInput(Thumbstick thumb, const ThumbstickInput &input);
	ThumbstickInput thumbstickInput(Thumbstick thumb) const;

	void setLeftThumbMouseMove(bool enabled);
	bool leftThumbMouseMove() const;
	void setRightThumbMouseMove(bool enabled);
	bool rightThumbMouseMove() const;

	void initializeDefaultMappings();

	// For direct access if needed
	std::map<GamepadButtons, InputKeyCode> buttonMappings;
	std::map<Thumbstick, ThumbstickInput> thumbstickMappings;

  private:
	void loadFromSettings(QSettings const &settings);
	void saveToSettings(QSettings &settings) const;
};
