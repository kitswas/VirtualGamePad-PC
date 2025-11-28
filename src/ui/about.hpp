#pragma once

#include <QStackedWidget>
#include <QWidget>

namespace Ui
{
class About;
}

class About : public QWidget
{
	Q_OBJECT

  signals:
	void navigateBack();

  public:
	explicit About(QStackedWidget *parent = nullptr);
	~About() override;

  private slots:
	void on_backButton_clicked();

  private:
	Ui::About *ui;
};
