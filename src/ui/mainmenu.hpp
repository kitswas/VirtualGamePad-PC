#pragma once

#include <QStackedWidget>
#include <QWidget>

namespace Ui
{
class MainMenu;
}

class MainMenu : public QWidget
{
	Q_OBJECT

  public:
	explicit MainMenu(QStackedWidget *parent);
	~MainMenu();

  private slots:
	void launch_server();
	void launch_preferences();

  private:
	Ui::MainMenu *ui;
	QStackedWidget *stack;
};
