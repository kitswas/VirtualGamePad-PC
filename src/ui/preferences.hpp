#pragma once

#include "../settings/input_types.hpp"
#include "../settings/settings_singleton.hpp"
#include "ButtonInputBox.hpp"

#include <QString>
#include <QWidget>
#include <minwindef.h>

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
	void new_profile();
	void delete_profile();
	void profile_selection_changed(const QString &profileName);
	void change_port(int value);
	// saveCurrentProfileChanges removed - no immediate saving

  private:
	void change_key_inputs();
	void change_thumbstick_inputs();
	void change_mouse_sensitivity(int value);
	void get_scan_code(WORD vk, char *a, int size = 256);
	void setup_profile_management();
	void refresh_profile_list();
	void setupKeymapTabs();
	bool load_profile_from_file(const QString &profilePath); // To be deprecated

	Ui::Preferences *ui;
	QString currentProfile;
};
