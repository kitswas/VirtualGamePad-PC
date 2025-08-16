#pragma once

#include "settings_singleton.hpp"

#include <QDir>

namespace setting_keys // a name space to maintain the key names.
{

enum server_keys
{
	Port
};

const QString Mouse_sensitivity = "mouse_setting/mouse_sensitivity";
const QString Executor_type = "general/executor_type";

enum button_keys
{
	A,
	B,
	X,
	Y,
	RSHDR,
	LSHDR,
	DPADDOWN,
	DPADUP,
	DPADRIGHT,
	DPADLEFT,
	VIEW,
	MENU
};

enum thumbstick_keys
{
	LeftThumbstick,
	LeftThumbstickUpKey,
	LeftThumbstickDownKey,
	LeftThumbstickLeftKey,
	LeftThumbstickRightKey,
	RightThumbstick,
	RightThumbstickUpKey,
	RightThumbstickDownKey,
	RightThumbstickLeftKey,
	RightThumbstickRightKey
};

enum trigger_keys
{
	LeftTriggerKey,
	LeftTriggerThreshold,
	RightTriggerKey,
	RightTriggerThreshold
};

} // namespace setting_keys

inline QList<QString> server_settings = {"port"};

/**
 * A Qmap to map the keys in namespace to corresponding settings name in string format.
 * Used for profile .ini files only, not for VirtualGamePad.ini.
 */
const inline QMap<setting_keys::button_keys, QString> button_settings = {
	{setting_keys::button_keys::A, "buttons/A"},
	{setting_keys::button_keys::B, "buttons/B"},
	{setting_keys::button_keys::X, "buttons/X"},
	{setting_keys::button_keys::Y, "buttons/Y"},
	{setting_keys::button_keys::RSHDR, "buttons/RS"},
	{setting_keys::button_keys::LSHDR, "buttons/LS"},
	{setting_keys::button_keys::DPADDOWN, "buttons/DPADDOWN"},
	{setting_keys::button_keys::DPADUP, "buttons/DPADUP"},
	{setting_keys::button_keys::DPADRIGHT, "buttons/DPADRIGHT"},
	{setting_keys::button_keys::DPADLEFT, "buttons/DPADLEFT"},
	{setting_keys::button_keys::VIEW, "buttons/VIEW"},
	{setting_keys::button_keys::MENU, "buttons/MENU"}};

/**
 * A QMap to map thumbstick keys to corresponding settings names in string format.
 * Used for profile .ini files only, not for VirtualGamePad.ini.
 */
const inline QMap<setting_keys::thumbstick_keys, QString> thumbstick_settings = {
	{setting_keys::thumbstick_keys::LeftThumbstick, "thumbsticks/LeftThumbstickMode"},
	{setting_keys::thumbstick_keys::LeftThumbstickUpKey, "thumbsticks/LeftThumbstickUp"},
	{setting_keys::thumbstick_keys::LeftThumbstickDownKey, "thumbsticks/LeftThumbstickDown"},
	{setting_keys::thumbstick_keys::LeftThumbstickLeftKey, "thumbsticks/LeftThumbstickLeft"},
	{setting_keys::thumbstick_keys::LeftThumbstickRightKey, "thumbsticks/LeftThumbstickRight"},
	{setting_keys::thumbstick_keys::RightThumbstick, "thumbsticks/RightThumbstickMode"},
	{setting_keys::thumbstick_keys::RightThumbstickUpKey, "thumbsticks/RightThumbstickUp"},
	{setting_keys::thumbstick_keys::RightThumbstickDownKey, "thumbsticks/RightThumbstickDown"},
	{setting_keys::thumbstick_keys::RightThumbstickLeftKey, "thumbsticks/RightThumbstickLeft"},
	{setting_keys::thumbstick_keys::RightThumbstickRightKey, "thumbsticks/RightThumbstickRight"}};

/**
 * A QMap to map trigger keys to corresponding settings names in string format.
 * Used for profile .ini files only, not for VirtualGamePad.ini.
 */
const inline QMap<setting_keys::trigger_keys, QString> trigger_settings = {
	{setting_keys::trigger_keys::LeftTriggerKey, "triggers/LeftTriggerKey"},
	{setting_keys::trigger_keys::LeftTriggerThreshold, "triggers/LeftTriggerThreshold"},
	{setting_keys::trigger_keys::RightTriggerKey, "triggers/RightTriggerKey"},
	{setting_keys::trigger_keys::RightTriggerThreshold, "triggers/RightTriggerThreshold"}};
