#pragma once

#include <QString>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class Preferences;
}
QT_END_NAMESPACE

class Preferences : public QWidget
{
	Q_OBJECT

  signals:
	void navigateBack();

  public:
	explicit Preferences(QWidget *parent = nullptr);
	~Preferences() override;

	void load_keys();
	void load_thumbsticks();
	void load_triggers();
	void load_port();
	void load_executor_type();
  private slots:
	void show_help();
	void new_profile();
	void delete_profile();
	void profile_selection_changed(const QString &profileName);
	void change_port(int value);
	void restore_defaults();
	void executor_type_changed();

  private:
	void change_key_inputs();
	void change_thumbstick_inputs();
	void change_trigger_inputs();
	void change_mouse_sensitivity(int value);
	void setup_profile_management();
	void refresh_profile_list();
	void setupKeymapTabs();
	void update_executor_notes();

	Ui::Preferences *ui;
	QString currentProfile;
};
