#include "buttoninputbox.hpp"

#include "../settings/settings_singleton.hpp"

#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>

ButtonInputBox::ButtonInputBox(QWidget *parent) : QLineEdit(parent)
{
	setReadOnly(false);
	setClearButtonEnabled(true);
	// Connect clear button signal
	connect(this, &QLineEdit::textChanged, this, [this](const QString &text) {
		if (text.isEmpty())
		{
			m_vk = 0;
		}
	});
}

KeyCodeType ButtonInputBox::keyCode() const
{
	return m_vk;
}

void ButtonInputBox::clearKeyCode()
{
	m_vk = 0;
	m_displayName.clear();
	clear();
}

QString ButtonInputBox::displayName() const
{
	return m_displayName;
}

void ButtonInputBox::setDisplayName(const QString &displayName)
{
	if (m_displayName != displayName)
	{
		m_displayName = displayName;
		updateDisplay();
	}
}

void ButtonInputBox::setKeyCodeAndDisplayName(KeyCodeType vk, const QString &displayName)
{
	if (m_vk != vk || m_displayName != displayName)
	{
		m_vk = vk;
		m_displayName = displayName;
		updateDisplay();
	}
}

void ButtonInputBox::updateDisplay()
{
	if (m_vk == 0)
	{
		m_displayName.clear();
		clear();
	}
	else
	{
		setText(m_displayName);
	}
}

bool ButtonInputBox::event(QEvent *event)
{
	// Check for clear button click
	if (event->type() == QEvent::MouseButtonRelease)
	{
		QMouseEvent const *mouseEvent = static_cast<QMouseEvent *>(event);
		// Check if click is in the clear button area
		if (!text().isEmpty() && mouseEvent->pos().x() > width() - 20)
		{
			clear();
			m_vk = 0;
			return true;
		}
	}
	else if (event->type() == QEvent::KeyPress)
	{
		keyPressEvent(static_cast<QKeyEvent *>(event));
		return true;
	}
	else if (event->type() == QEvent::MouseButtonPress)
	{
		mousePressEvent(static_cast<QMouseEvent *>(event));
		return true;
	}
	return QLineEdit::event(event);
}

void ButtonInputBox::keyPressEvent(QKeyEvent *event)
{
	// Only capture key if field is empty
	if (!text().isEmpty())
	{
		qDebug() << "Ignoring key press - field not empty";
		return;
	}

#ifdef __linux__
	// Linux: use nativeScanCode() for evdev keycodes (see header for details)
	m_vk = event->nativeScanCode() - 8; // evdev codes are offset by 8
#else
	// Windows: use nativeVirtualKey() for VK_* codes
	m_vk = event->nativeVirtualKey();
#endif
	m_displayName = QKeySequence(event->key()).toString(QKeySequence::PortableText);
	qDebug() << "Key captured - Qt Key code:" << event->key() << "Native virtual key code"
			 << event->nativeVirtualKey() << "Native scan code" << event->nativeScanCode()
			 << "Key:" << m_displayName;

	updateDisplay();
}

void ButtonInputBox::mousePressEvent(QMouseEvent *event)
{
	// Only process mouse buttons when the field is empty
	if (text().isEmpty())
	{
		switch (event->button())
		{
		case Qt::LeftButton:
			m_vk = Qt::LeftButton;
			m_displayName = "Left Button";
			qDebug() << "Left mouse button captured";
			break;
		case Qt::RightButton:
			m_vk = Qt::RightButton;
			m_displayName = "Right Button";
			qDebug() << "Right mouse button captured";
			break;
		case Qt::MiddleButton:
			m_vk = Qt::MiddleButton;
			m_displayName = "Middle Button";
			qDebug() << "Middle mouse button captured";
			break;
		default:
			qDebug() << "Unsupported mouse button ignored:" << event->button();
			return;
		}

		updateDisplay();
	}

	// Always set focus when clicking on the field
	setFocus();
}
