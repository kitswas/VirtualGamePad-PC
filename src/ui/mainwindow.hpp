#pragma once

#include "mainmenu.hpp"

#include <QMainWindow>

namespace Ui
{
class MainWindow;
}

class MainWindow final : public QMainWindow
{
	Q_OBJECT

  public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow() override;

  private:
	Ui::MainWindow *ui;
};
