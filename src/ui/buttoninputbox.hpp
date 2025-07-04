#pragma once

#include <QEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMouseEvent>
#include <Qt>

typedef quint32 KeyCodeType; // Native virtual key code from QKeyEvent::nativeVirtualKey()

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
	QString m_displayName;
	void updateDisplay();
};
