#pragma once

#include <QEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMouseEvent>
#include <Qt>
#include <map>

// Use Qt's key system for platform independence
using KeyCodeType = int; // Qt::Key values are int

class ButtonInputBox : public QLineEdit
{
	Q_OBJECT
  public:
	explicit ButtonInputBox(QWidget *parent = nullptr);
	~ButtonInputBox() override = default;
	KeyCodeType keyCode() const;
	void setKeyCode(KeyCodeType vk);
	void clearKeyCode();

  signals:
	void keyCodeChanged(KeyCodeType vk);

  protected:
	bool event(QEvent *event) override;
	void keyPressEvent(QKeyEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;

  private:
	KeyCodeType m_vk = 0;
	void updateDisplay();
	static QString getKeyName(KeyCodeType keyCode);
};
