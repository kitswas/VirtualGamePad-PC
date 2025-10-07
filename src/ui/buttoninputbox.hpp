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
	void clearKeyCode();
	QString displayName() const;
	void setDisplayName(const QString &displayName);
	void setKeyCodeAndDisplayName(KeyCodeType vk, const QString &displayName);

  protected:
	bool event(QEvent *event) override;
	void keyPressEvent(QKeyEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;

  private:
	KeyCodeType m_vk = 0;
	QString m_displayName;
	void updateDisplay();
};
