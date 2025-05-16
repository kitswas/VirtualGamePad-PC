#pragma once

#include "../settings/input_types.hpp"
#include "../settings/settings_singleton.hpp"
#include <QMap>
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

  protected:
	bool eventFilter(QObject *sender, QEvent *event) override;
	void keyPressEvent(QKeyEvent *e) override;

  private slots:
	void show_help();
	void load_profile();
	void save_profile();
	void new_profile();
	void delete_profile();
	void profile_selection_changed(const QString &profileName);

  private:
	void change_key_inputs();
	void change_thumbstick_inputs();
	void change_mouse_sensitivity(int value);
	void get_scan_code(WORD vk, char *a, int size = 256);
	void install_event_filter();
	void load_thumbstick_keys();
	void setup_profile_management();
	void refresh_profile_list();
	void save_profile_to_file(const QString &profilePath);
	bool load_profile_from_file(const QString &profilePath);
	QString get_profiles_dir() const;

	void setupKeymapTabs();

	Ui::Preferences *ui;
	std::map<QString, WORD> temp;
	QString currentProfile;
};
