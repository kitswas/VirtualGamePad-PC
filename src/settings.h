#ifndef SETTINGS_H
#define SETTINGS_H
#include <QDir>
#include <QSettings>
#include <minwindef.h>

extern QString SETTINGS_FILE;

namespace setting_keys // a name space to maintain the key names.
{

enum server_keys
{
	Port
};

extern QString Mouse_sensivity;

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
    DPADLEFT
};

} // namespace setting_keys

extern QSettings *settings;
inline QList<QString> server_settings = {"port"};
//inline QList<QString> keymaps = {"keymaps/A", "keymaps/B", "keymaps/X", "keymaps/Y", "keymaps/RT", "keymaps/LT"};
/**
 * @brief keymaps
 * a Qmap to map the keys in namespace to corresponding settings name in string format.
 */
inline QMap<setting_keys::keys, QString> keymaps = {{setting_keys::keys::A, "keymaps/A"},
                                                    {setting_keys::keys::B, "keymaps/B"},
                                                    {setting_keys::keys::X, "keymaps/X"},
                                                    {setting_keys::keys::Y, "keymaps/Y"},
													{setting_keys::keys::RSHDR, "keymaps/RT"},
													{setting_keys::keys::LSHDR, "keymaps/LT"},
                                                    {setting_keys::keys::DPADDOWN, "keymaps/DPADDOWN"},
                                                    {setting_keys::keys::DPADUP, "keymaps/DPADUP"},
                                                    {setting_keys::keys::DPADRIGHT, "keymaps/DPADRIGHT"},
                                                    {setting_keys::keys::DPADLEFT, "keymaps/DPADLEFT"}};


void save_setting(QString key, QVariant value);
QVariant load_setting(QString key);
void load_mouse_setting();
void set_mouse_sensivity();
void load_settings_file(QObject *parent);
void load_port_number();
void load_key_maps();
void load_all_settings();
uint is_mouse_button(UINT vk);

#endif // SETTINGS_H
