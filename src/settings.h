#ifndef SETTINGS_H
#define SETTINGS_H

#endif // SETTINGS_H
#include <QDir>
#include <QSettings>

extern QString SETTINGS_FILE;

namespace setting_keys
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
	RT,
	LT
};

} // namespace setting_keys

extern QSettings *settings;
inline QList<QString> server_settings = {"port"};
inline QList<QString> keymaps = {"keymaps/A", "keymaps/B", "keymaps/X", "keymaps/Y", "keymaps/RT", "keymaps/LT"};

void save_setting(QString key, QVariant value);
QVariant load_setting(QString key);
void load_mouse_setting();
void load_settings_file(QObject *parent);
void load_port_number();
