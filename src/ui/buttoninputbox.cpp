#include "ButtonInputBox.hpp"

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

WORD ButtonInputBox::keyCode() const
{
	return m_vk;
}

void ButtonInputBox::setKeyCode(WORD vk)
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
		QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
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
	// Handle Escape key to clear the field
	if (event->key() == Qt::Key_Escape)
	{
		clear();
		m_vk = 0;
		emit keyCodeChanged(m_vk);
		return;
	}

	// Only capture key if field is empty
	if (!text().isEmpty())
	{
		// Ignore keyboard input if not empty
		return;
	}

	m_vk = event->nativeVirtualKey();
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
			m_vk = VK_LBUTTON;
			break;
		case Qt::RightButton:
			m_vk = VK_RBUTTON;
			break;
		case Qt::MiddleButton:
			m_vk = VK_MBUTTON;
			break;
		default:
			return; // Ignore other buttons
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

	const auto &map = vkMap();
	auto it = map.find(m_vk);
	if (it != map.end())
	{
		setText(QString::fromLatin1(it->second));
	}
	else if (m_vk >= '0' && m_vk <= 'Z')
	{
		setText(QChar(static_cast<char>(m_vk)));
	}
	else
	{
		setText(QString::number(m_vk));
	}
}

const std::map<WORD, const char *> &ButtonInputBox::vkMap()
{
	return vk_maps;
}
