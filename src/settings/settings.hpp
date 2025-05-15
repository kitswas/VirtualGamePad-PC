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

#endif // SETTINGS_H
