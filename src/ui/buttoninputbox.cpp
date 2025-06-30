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
			emit keyCodeChanged(m_vk);
		}
	});
}

KeyCodeType ButtonInputBox::keyCode() const
{
	return m_vk;
}

void ButtonInputBox::setKeyCode(KeyCodeType vk)
{
	if (m_vk != vk)
	{
		m_vk = vk;
		updateDisplay();
		emit keyCodeChanged(m_vk);
	}
}

void ButtonInputBox::clearKeyCode()
{
	m_vk = 0;
	clear();
	emit keyCodeChanged(m_vk);
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
			emit keyCodeChanged(m_vk);
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

	m_vk = event->key(); // Use Qt::Key enum directly
	qDebug() << "Key captured - Qt Key code:" << m_vk
			 << "Key:" << QKeySequence(event->key()).toString();

	updateDisplay();
	emit keyCodeChanged(m_vk);
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
			qDebug() << "Left mouse button captured";
			break;
		case Qt::RightButton:
			m_vk = Qt::RightButton;
			qDebug() << "Right mouse button captured";
			break;
		case Qt::MiddleButton:
			m_vk = Qt::MiddleButton;
			qDebug() << "Middle mouse button captured";
			break;
		default:
			qDebug() << "Unsupported mouse button ignored:" << event->button();
			return;
		}

		updateDisplay();
		emit keyCodeChanged(m_vk);
	}

	// Always set focus when clicking on the field
	setFocus();
}

void ButtonInputBox::updateDisplay()
{
	if (m_vk == 0)
	{
		clear();
		return;
	}

	setText(getKeyName(m_vk));
}

QString ButtonInputBox::getKeyName(KeyCodeType keyCode)
{
	// Handle mouse buttons
	if (keyCode == Qt::LeftButton)
		return "LMButton";
	if (keyCode == Qt::RightButton)
		return "RMButton";
	if (keyCode == Qt::MiddleButton)
		return "MMButton";

	// Handle keyboard keys using Qt's key sequence system
	QKeySequence sequence(keyCode);
	QString keyName = sequence.toString();

	if (!keyName.isEmpty())
	{
		return keyName;
	}

	// Fallback to showing the key code number
	return QString::number(keyCode);
}
