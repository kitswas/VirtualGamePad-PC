#ifndef SETTINGS_H
#define SETTINGS_H
#include "settings_singleton.hpp"

#include <QDir>
#include <minwindef.h>

extern const QString SETTINGS_FILE;

namespace setting_keys // a name space to maintain the key names.
{

enum server_keys
{
	Port
};

const QString Mouse_sensitivity = "mouse_setting/mouse_sensitivity";

enum keys
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

// Add enum for thumbstick setting keys
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

} // namespace setting_keys

inline QList<QString> server_settings = {"port"};

/**
 * A Qmap to map the keys in namespace to corresponding settings name in string format.
 */
const inline QMap<setting_keys::keys, QString> keymaps = {
	{setting_keys::keys::A, "keymaps/A"},
	{setting_keys::keys::B, "keymaps/B"},
	{setting_keys::keys::X, "keymaps/X"},
	{setting_keys::keys::Y, "keymaps/Y"},
	{setting_keys::keys::RSHDR, "keymaps/RT"},
	{setting_keys::keys::LSHDR, "keymaps/LT"},
	{setting_keys::keys::DPADDOWN, "keymaps/DPADDOWN"},
	{setting_keys::keys::DPADUP, "keymaps/DPADUP"},
	{setting_keys::keys::DPADRIGHT, "keymaps/DPADRIGHT"},
	{setting_keys::keys::DPADLEFT, "keymaps/DPADLEFT"},
	{setting_keys::keys::VIEW, "keymaps/VIEW"},
	{setting_keys::keys::MENU, "keymaps/MENU"}};

/**
 * A QMap to map thumbstick keys to corresponding settings names in string format.
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

#endif // SETTINGS_H
