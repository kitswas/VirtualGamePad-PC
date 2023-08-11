#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>

namespace Ui
{
class Preferences;
}

class Preferences : public QDialog
{
	Q_OBJECT

  public:
	explicit Preferences(QWidget *parent = nullptr);
	~Preferences();

  private:
	Ui::Preferences *ui;
	int change_mouse_sensitivity(int value);
};

#endif // PREFERENCES_H
