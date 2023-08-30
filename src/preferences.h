#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>
#include <minwindef.h>

namespace Ui
{
class Preferences;
}

class Preferences : public QDialog
{
	Q_OBJECT

  public:
    explicit Preferences(QWidget *parent = nullptr);
    void load_keys();
	~Preferences();

  protected:
    bool eventFilter(QObject* sender, QEvent* event);
    void keyPressEvent(QKeyEvent  *e);

  private:
	Ui::Preferences *ui;
	int change_mouse_sensitivity(int value);
    void change_key_inputs();
    void get_scan_code(WORD vk, char* a, int size=256);
    void install_event_filter();
    UINT X;
    UINT Y;
    UINT A;
    UINT B;
    UINT LT;
    UINT RT;
    UINT DUP;
    UINT DDOWN;
    UINT DRIGHT;
    UINT DLEFT;
};

#endif // PREFERENCES_H
