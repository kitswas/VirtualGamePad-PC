#pragma once

#include "../settings/input_types.hpp"
#include "../settings/settings_singleton.hpp"
#include "ButtonInputBox.hpp"

#include <QString>
#include <QWidget>
#include <windows.h>

QT_BEGIN_NAMESPACE
namespace Ui
{
class Preferences;
}
QT_END_NAMESPACE

class Preferences : public QWidget
{
	Q_OBJECT

  public:
	explicit Preferences(QWidget *parent = nullptr);
	~Preferences();

	void load_keys();
	void load_thumbsticks();
	void load_port();

  private slots:
	void show_help();
	// void load_profile(); // Remove this slot
	void save_profile();
	void new_profile();
	void delete_profile();
	void profile_selection_changed(const QString &profileName);
	void change_port(int value);

  private:
	void change_key_inputs();
	void change_thumbstick_inputs();
	void change_mouse_sensitivity(int value);
	void get_scan_code(WORD vk, char *a, int size = 256);
	void load_thumbstick_keys();
	void setup_profile_management();
	void refresh_profile_list();
	void save_profile_to_file(const QString &profilePath);
	bool load_profile_from_file(const QString &profilePath);
	QString get_profiles_dir() const;

	void setupKeymapTabs();

	// New: Save/load active profile name to main settings file
	void save_active_profile_name(const QString &profileName);
	QString load_active_profile_name();

	Ui::Preferences *ui;
	QString currentProfile;

	// All key/axis mapping widgets are now ButtonInputBox*
	// Note: Key/axis mappings are only saved to profiles, not to VirtualGamePad.ini.
};
