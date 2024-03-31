#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>
#include <Windows.h>
#include <minwindef.h>

namespace Ui
{
class Preferences;
}

class Preferences final : public QDialog
{
	Q_OBJECT

  public:
	explicit Preferences(QWidget *parent = nullptr);
	void load_keys();
	~Preferences() override;

  protected:
	bool eventFilter(QObject *sender, QEvent *event) override;
	void keyPressEvent(QKeyEvent *e) override;

  private:
	Ui::Preferences *ui;
	void change_mouse_sensitivity(int value);
	void change_key_inputs();
	void get_scan_code(WORD vk, char *a, int size = 256);
	void install_event_filter();
	std::map<QString, UINT> temp;
};

#endif // PREFERENCES_H
