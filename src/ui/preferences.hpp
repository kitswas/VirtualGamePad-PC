#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QWidget>
#include <minwindef.h>
#include <windows.h>

namespace Ui
{
class Preferences;
}

class Preferences : public QWidget
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
