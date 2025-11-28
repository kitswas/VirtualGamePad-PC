#pragma once

#include <QEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMouseEvent>
#include <Qt>

/**
 * @brief Platform-native key code type.
 *
 * On Windows: Windows virtual key codes (VK_*) from QKeyEvent::nativeVirtualKey()
 * On Linux: evdev keycodes (KEY_*) from QKeyEvent::nativeScanCode()
 *
 * @see ButtonInputBox::keyPressEvent() for details on platform differences.
 */
typedef quint32 KeyCodeType;

/**
 * @brief A QLineEdit subclass for capturing keyboard and mouse input for key mapping.
 *
 * This widget captures a single key press or mouse button click and stores the
 * platform-native key code for use in input simulation.
 */
class ButtonInputBox : public QLineEdit
{
	Q_OBJECT
  public:
	explicit ButtonInputBox(QWidget *parent = nullptr);
	~ButtonInputBox() override = default;

	/// @brief Returns the captured platform-native key code.
	KeyCodeType keyCode() const;

	/// @brief Clears the captured key code and display name.
	void clearKeyCode();

	/// @brief Returns the human-readable display name of the captured key.
	QString displayName() const;

	/// @brief Sets the display name without changing the key code.
	void setDisplayName(const QString &displayName);

	/// @brief Sets both the key code and display name.
	void setKeyCodeAndDisplayName(KeyCodeType vk, const QString &displayName);

  protected:
	bool event(QEvent *event) override;

	/**
	 * @brief Captures a key press and stores the platform-native key code.
	 *
	 * @par Platform-Specific Behavior
	 *
	 * Qt provides two native key identifiers, but their meaning differs by platform:
	 *
	 * @par Linux (X11/XCB):
	 * - `nativeVirtualKey()` returns X11 keysyms (defined in X11/keysymdef.h)
	 *   - For Latin letters, these match ASCII codes (e.g., XK_k = 0x006b = 107)
	 *   - Represent the logical/semantic meaning of a key
	 * - `nativeScanCode()` returns evdev keycodes (defined in linux/input-event-codes.h)
	 *   - Hardware-oriented codes for physical key positions (e.g., KEY_K = 37)
	 *   - Used by Linux kernel input subsystem (evdev/uinput)
	 *
	 * Since KeyboardInjector uses libevdev/uinput, we use `nativeScanCode()` on Linux.
	 *
	 * @par Windows:
	 * - `nativeVirtualKey()` returns Windows virtual key codes (VK_*)
	 * - These are directly usable with SendInput API
	 *
	 * @par Example Mappings (Linux)
	 * | Key   | nativeVirtualKey() | nativeScanCode() |
	 * |-------|-------------------|------------------|
	 * | K     | 107 (XK_k)        | 37 (KEY_K)       |
	 * | A     | 97 (XK_a)         | 30 (KEY_A)       |
	 * | Enter | 0xff0d (XK_Return)| 28 (KEY_ENTER)   |
	 *
	 * @see https://doc.qt.io/qt-6/qkeyevent.html#nativeVirtualKey
	 * @see https://wiki.archlinux.org/title/Keyboard_input
	 */
	void keyPressEvent(QKeyEvent *event) override;

	void mousePressEvent(QMouseEvent *event) override;

  private:
	KeyCodeType m_vk = 0;
	QString m_displayName;
	void updateDisplay();
};
