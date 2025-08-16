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

	InputKeyCode buttonMap(GamepadButtons btn) const;

	// Methods for display names
	void setButtonInput(GamepadButtons btn, InputKeyCode vk, const QString &displayName);
	QString buttonDisplayName(GamepadButtons btn) const;

	ButtonInput buttonInput(GamepadButtons button) const;

	void setThumbstickInput(Thumbstick thumb, const ThumbstickInput &input);
	ThumbstickInput thumbstickInput(Thumbstick thumb) const;

	void setTriggerInput(Trigger trigger, const TriggerInput &input);
	TriggerInput triggerInput(Trigger trigger) const;

	void setLeftThumbMouseMove(bool enabled);
	bool leftThumbMouseMove() const;
	void setRightThumbMouseMove(bool enabled);
	bool rightThumbMouseMove() const;

	void initializeDefaultMappings();

	// For direct access if needed
	std::map<GamepadButtons, InputKeyCode> buttonMappings;
	std::map<GamepadButtons, QString> buttonDisplayNames;
	std::map<Thumbstick, ThumbstickInput> thumbstickMappings;
	std::map<Trigger, TriggerInput> triggerMappings;

  private:
	void loadFromSettings(QSettings const &settings);
	void saveToSettings(QSettings &settings) const;
};
