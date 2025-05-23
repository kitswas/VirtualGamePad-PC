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
	~MainMenu() override;

  private slots:
	void launch_server();
	void launch_preferences();
	void launch_about();

  private:
	Ui::MainMenu *ui;
	QStackedWidget *stack;
};
