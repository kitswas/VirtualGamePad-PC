#pragma once
#include <QEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMouseEvent>
#include <map>
#include <minwindef.h>

class ButtonInputBox : public QLineEdit
{
	Q_OBJECT
  public:
	explicit ButtonInputBox(QWidget *parent = nullptr);
	~ButtonInputBox() override = default;
	WORD keyCode() const;
	void setKeyCode(WORD vk);
	void clearKeyCode();

  signals:
	void keyCodeChanged(WORD vk);

  protected:
	bool event(QEvent *event) override;
	void keyPressEvent(QKeyEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;

  private:
	WORD m_vk = 0;
	void updateDisplay();
	static const std::map<WORD, const char *> &vkMap();
};
