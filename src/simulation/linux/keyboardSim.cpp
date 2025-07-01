#include "../keyboardSim.hpp"

#include <Qt>
#include <QDebug>
#include <QThread>
#include <QKeySequence>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

// Static member initialization
std::unique_ptr<libevdev_uinput, void(*)(libevdev_uinput*)> KeyboardInjector::s_keyboardDevice(nullptr, libevdev_uinput_destroy);

std::unique_ptr<libevdev_uinput, void(*)(libevdev_uinput*)> KeyboardInjector::getKeyboardDevice()
{
    if (s_keyboardDevice) {
        return std::unique_ptr<libevdev_uinput, void(*)(libevdev_uinput*)>(nullptr, libevdev_uinput_destroy);
    }

    // Create a new libevdev device for keyboard
    libevdev* dev = libevdev_new();
    if (!dev) {
        qCritical() << "Failed to create keyboard libevdev device";
        return std::unique_ptr<libevdev_uinput, void(*)(libevdev_uinput*)>(nullptr, libevdev_uinput_destroy);
    }

    // Set device properties
    libevdev_set_name(dev, "Virtual Gamepad PC Keyboard");
    libevdev_set_id_bustype(dev, BUS_USB);
    libevdev_set_id_vendor(dev, 0x1d6b);  // Linux Foundation
    libevdev_set_id_product(dev, 0x0001); // Generic keyboard
    libevdev_set_id_version(dev, 0x0100);

    // Enable keyboard events
    libevdev_enable_event_type(dev, EV_KEY);
    libevdev_enable_event_type(dev, EV_REP); // Key repeat

    // Enable all keyboard keys (we'll map Qt key codes to Linux key codes)
    for (int i = 0; i < KEY_MAX; i++) {
        if (i >= KEY_ESC && i <= KEY_COMPOSE) {
            libevdev_enable_event_code(dev, EV_KEY, i, nullptr);
        }
    }

    // Create uinput device
    libevdev_uinput* uidev;
    int ret = libevdev_uinput_create_from_device(dev, LIBEVDEV_UINPUT_OPEN_MANAGED, &uidev);
    libevdev_free(dev);

    if (ret < 0) {
        qCritical() << "Failed to create keyboard uinput device:" << strerror(-ret);
        return std::unique_ptr<libevdev_uinput, void(*)(libevdev_uinput*)>(nullptr, libevdev_uinput_destroy);
    }

    s_keyboardDevice.reset(uidev);
    qDebug() << "Virtual keyboard device created successfully";
    
    return std::unique_ptr<libevdev_uinput, void(*)(libevdev_uinput*)>(nullptr, libevdev_uinput_destroy);
}

/**
 * Reference: https://doc.qt.io/qt-6/qt.html#Key-enum
 */
int KeyboardInjector::qtKeyToLinuxKey(int qtKey)
{
    // Handle mouse buttons using Qt constants
    if (qtKey == Qt::LeftButton || qtKey == Qt::RightButton || qtKey == Qt::MiddleButton) {
        // Mouse buttons don't have keyboard equivalents
        qWarning() << "Mouse button not supported in keyboard simulation:" << qtKey;
        return KEY_RESERVED;
    }

    // Map Qt::Key values to Linux key codes
    // Note: Linux input constants (KEY_*) are required for libevdev compatibility
    switch (qtKey) {
        // Letters - mapping Qt::Key enum to Linux input constants
        case Qt::Key_A: return KEY_A;
        case Qt::Key_B: return KEY_B;
        case Qt::Key_C: return KEY_C;
        case Qt::Key_D: return KEY_D;
        case Qt::Key_E: return KEY_E;
        case Qt::Key_F: return KEY_F;
        case Qt::Key_G: return KEY_G;
        case Qt::Key_H: return KEY_H;
        case Qt::Key_I: return KEY_I;
        case Qt::Key_J: return KEY_J;
        case Qt::Key_K: return KEY_K;
        case Qt::Key_L: return KEY_L;
        case Qt::Key_M: return KEY_M;
        case Qt::Key_N: return KEY_N;
        case Qt::Key_O: return KEY_O;
        case Qt::Key_P: return KEY_P;
        case Qt::Key_Q: return KEY_Q;
        case Qt::Key_R: return KEY_R;
        case Qt::Key_S: return KEY_S;
        case Qt::Key_T: return KEY_T;
        case Qt::Key_U: return KEY_U;
        case Qt::Key_V: return KEY_V;
        case Qt::Key_W: return KEY_W;
        case Qt::Key_X: return KEY_X;
        case Qt::Key_Y: return KEY_Y;
        case Qt::Key_Z: return KEY_Z;

        // Numbers
        case Qt::Key_0: return KEY_0;
        case Qt::Key_1: return KEY_1;
        case Qt::Key_2: return KEY_2;
        case Qt::Key_3: return KEY_3;
        case Qt::Key_4: return KEY_4;
        case Qt::Key_5: return KEY_5;
        case Qt::Key_6: return KEY_6;
        case Qt::Key_7: return KEY_7;
        case Qt::Key_8: return KEY_8;
        case Qt::Key_9: return KEY_9;

        // Function keys
        case Qt::Key_F1: return KEY_F1;
        case Qt::Key_F2: return KEY_F2;
        case Qt::Key_F3: return KEY_F3;
        case Qt::Key_F4: return KEY_F4;
        case Qt::Key_F5: return KEY_F5;
        case Qt::Key_F6: return KEY_F6;
        case Qt::Key_F7: return KEY_F7;
        case Qt::Key_F8: return KEY_F8;
        case Qt::Key_F9: return KEY_F9;
        case Qt::Key_F10: return KEY_F10;
        case Qt::Key_F11: return KEY_F11;
        case Qt::Key_F12: return KEY_F12;

        // Special keys
        case Qt::Key_Backspace: return KEY_BACKSPACE;
        case Qt::Key_Tab: return KEY_TAB;
        case Qt::Key_Return: 
        case Qt::Key_Enter: return KEY_ENTER;
        case Qt::Key_Shift: return KEY_LEFTSHIFT;
        case Qt::Key_Control: return KEY_LEFTCTRL;
        case Qt::Key_Alt: return KEY_LEFTALT;
        case Qt::Key_Escape: return KEY_ESC;
        case Qt::Key_Space: return KEY_SPACE;
        case Qt::Key_CapsLock: return KEY_CAPSLOCK;
        case Qt::Key_NumLock: return KEY_NUMLOCK;
        case Qt::Key_ScrollLock: return KEY_SCROLLLOCK;

        // Arrow keys
        case Qt::Key_Left: return KEY_LEFT;
        case Qt::Key_Up: return KEY_UP;
        case Qt::Key_Right: return KEY_RIGHT;
        case Qt::Key_Down: return KEY_DOWN;

        // Home cluster
        case Qt::Key_Insert: return KEY_INSERT;
        case Qt::Key_Delete: return KEY_DELETE;
        case Qt::Key_Home: return KEY_HOME;
        case Qt::Key_End: return KEY_END;
        case Qt::Key_PageUp: return KEY_PAGEUP;
        case Qt::Key_PageDown: return KEY_PAGEDOWN;

        // Punctuation
        case Qt::Key_Period: return KEY_DOT;
        case Qt::Key_Comma: return KEY_COMMA;
        case Qt::Key_Minus: return KEY_MINUS;
        case Qt::Key_Plus: return KEY_KPPLUS;
        case Qt::Key_Semicolon: return KEY_SEMICOLON;
        case Qt::Key_Apostrophe: return KEY_APOSTROPHE;
        case Qt::Key_BracketLeft: return KEY_LEFTBRACE;
        case Qt::Key_BracketRight: return KEY_RIGHTBRACE;
        case Qt::Key_Backslash: return KEY_BACKSLASH;
        case Qt::Key_Slash: return KEY_SLASH;

        // Special characters
        case Qt::Key_Equal: return KEY_EQUAL;
        case Qt::Key_QuoteLeft: return KEY_GRAVE;

        // Media keys
        case Qt::Key_VolumeUp: return KEY_VOLUMEUP;
        case Qt::Key_VolumeDown: return KEY_VOLUMEDOWN;
        case Qt::Key_VolumeMute: return KEY_MUTE;

        // Menu key
        case Qt::Key_Menu: return KEY_MENU;

        default:
            qWarning() << "Unmapped Qt key:" << qtKey << "(" << QKeySequence(static_cast<Qt::Key>(qtKey)).toString() << ")";
            return KEY_RESERVED; // Unknown key
    }
}

void KeyboardInjector::pressKey(int qtKeyCode)
{
    getKeyboardDevice(); // Ensure device is created
    if (!s_keyboardDevice) return;

    int linuxKey = qtKeyToLinuxKey(qtKeyCode);
    if (linuxKey == KEY_RESERVED) return;

    // Press key
    libevdev_uinput_write_event(s_keyboardDevice.get(), EV_KEY, linuxKey, 1);
    libevdev_uinput_write_event(s_keyboardDevice.get(), EV_SYN, SYN_REPORT, 0);

    // Wait
    QThread::msleep(PRESS_INTERVAL);

    // Release key
    libevdev_uinput_write_event(s_keyboardDevice.get(), EV_KEY, linuxKey, 0);
    libevdev_uinput_write_event(s_keyboardDevice.get(), EV_SYN, SYN_REPORT, 0);
}

void KeyboardInjector::pressKeyCombo(std::vector<int> qtKeys)
{
    getKeyboardDevice(); // Ensure device is created
    if (!s_keyboardDevice) return;

    // Press all keys
    for (int qtKeyCode : qtKeys) {
        int linuxKey = qtKeyToLinuxKey(qtKeyCode);
        if (linuxKey != KEY_RESERVED) {
            libevdev_uinput_write_event(s_keyboardDevice.get(), EV_KEY, linuxKey, 1);
        }
    }
    libevdev_uinput_write_event(s_keyboardDevice.get(), EV_SYN, SYN_REPORT, 0);

    // Wait
    QThread::msleep(PRESS_INTERVAL);

    // Release all keys
    for (int qtKeyCode : qtKeys) {
        int linuxKey = qtKeyToLinuxKey(qtKeyCode);
        if (linuxKey != KEY_RESERVED) {
            libevdev_uinput_write_event(s_keyboardDevice.get(), EV_KEY, linuxKey, 0);
        }
    }
    libevdev_uinput_write_event(s_keyboardDevice.get(), EV_SYN, SYN_REPORT, 0);
}

void KeyboardInjector::keyDown(int qtKeyCode)
{
    getKeyboardDevice(); // Ensure device is created
    if (!s_keyboardDevice) return;

    int linuxKey = qtKeyToLinuxKey(qtKeyCode);
    if (linuxKey == KEY_RESERVED) return;

    libevdev_uinput_write_event(s_keyboardDevice.get(), EV_KEY, linuxKey, 1);
    libevdev_uinput_write_event(s_keyboardDevice.get(), EV_SYN, SYN_REPORT, 0);
}

void KeyboardInjector::keyUp(int qtKeyCode)
{
    getKeyboardDevice(); // Ensure device is created
    if (!s_keyboardDevice) return;

    int linuxKey = qtKeyToLinuxKey(qtKeyCode);
    if (linuxKey == KEY_RESERVED) return;

    libevdev_uinput_write_event(s_keyboardDevice.get(), EV_KEY, linuxKey, 0);
    libevdev_uinput_write_event(s_keyboardDevice.get(), EV_SYN, SYN_REPORT, 0);
}

void KeyboardInjector::keyComboUp(std::vector<int> qtKeys)
{
    getKeyboardDevice(); // Ensure device is created
    if (!s_keyboardDevice) return;

    for (int qtKeyCode : qtKeys) {
        int linuxKey = qtKeyToLinuxKey(qtKeyCode);
        if (linuxKey != KEY_RESERVED) {
            libevdev_uinput_write_event(s_keyboardDevice.get(), EV_KEY, linuxKey, 0);
        }
    }
    libevdev_uinput_write_event(s_keyboardDevice.get(), EV_SYN, SYN_REPORT, 0);
}

void KeyboardInjector::keyComboDown(std::vector<int> qtKeys)
{
    getKeyboardDevice(); // Ensure device is created
    if (!s_keyboardDevice) return;

    for (int qtKeyCode : qtKeys) {
        int linuxKey = qtKeyToLinuxKey(qtKeyCode);
        if (linuxKey != KEY_RESERVED) {
            libevdev_uinput_write_event(s_keyboardDevice.get(), EV_KEY, linuxKey, 1);
        }
    }
    libevdev_uinput_write_event(s_keyboardDevice.get(), EV_SYN, SYN_REPORT, 0);
}

void KeyboardInjector::typeUnicodeString(const QString& str)
{
    getKeyboardDevice(); // Ensure device is created
    if (!s_keyboardDevice) return;

    for (const QChar& c : str) {
        if (c.isLetter()) {
            // Handle letters using Qt functions
            QChar upper = c.toUpper();
            Qt::Key qtKey = static_cast<Qt::Key>(upper.unicode());
            
            if (qtKey >= Qt::Key_A && qtKey <= Qt::Key_Z) {
                if (c.isUpper()) {
                    // Upper case - press shift + letter
                    std::vector<int> combo = {Qt::Key_Shift, static_cast<int>(qtKey)};
                    pressKeyCombo(combo);
                } else {
                    // Lower case - just press the letter
                    pressKey(static_cast<int>(qtKey));
                }
            }
        } else if (c.isDigit()) {
            // Handle digits using Qt functions
            int digitValue = c.digitValue();
            if (digitValue >= 0 && digitValue <= 9) {
                Qt::Key qtKey = static_cast<Qt::Key>(Qt::Key_0 + digitValue);
                pressKey(static_cast<int>(qtKey));
            }
        } else if (c.isSpace()) {
            pressKey(Qt::Key_Space);
        } else if (c == QChar::LineFeed || c == QChar::CarriageReturn) {
            pressKey(Qt::Key_Return);
        } else if (c == QChar::Tabulation) {
            pressKey(Qt::Key_Tab);
        }
        // Could add more character mappings using Qt character classification functions
    }
}
