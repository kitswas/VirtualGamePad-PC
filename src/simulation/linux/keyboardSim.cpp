#include "../keyboardSim.hpp"

#include <QDebug>
#include <QThread>
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

// Helper function to map Windows VK codes to Linux key codes
int windowsVKToLinuxKey(int vkCode)
{
    // This is a simplified mapping - you may need to extend this for full compatibility
    switch (vkCode) {
        // Letters
        case 0x41: return KEY_A;  // VK_A
        case 0x42: return KEY_B;  // VK_B
        case 0x43: return KEY_C;  // VK_C
        case 0x44: return KEY_D;  // VK_D
        case 0x45: return KEY_E;  // VK_E
        case 0x46: return KEY_F;  // VK_F
        case 0x47: return KEY_G;  // VK_G
        case 0x48: return KEY_H;  // VK_H
        case 0x49: return KEY_I;  // VK_I
        case 0x4A: return KEY_J;  // VK_J
        case 0x4B: return KEY_K;  // VK_K
        case 0x4C: return KEY_L;  // VK_L
        case 0x4D: return KEY_M;  // VK_M
        case 0x4E: return KEY_N;  // VK_N
        case 0x4F: return KEY_O;  // VK_O
        case 0x50: return KEY_P;  // VK_P
        case 0x51: return KEY_Q;  // VK_Q
        case 0x52: return KEY_R;  // VK_R
        case 0x53: return KEY_S;  // VK_S
        case 0x54: return KEY_T;  // VK_T
        case 0x55: return KEY_U;  // VK_U
        case 0x56: return KEY_V;  // VK_V
        case 0x57: return KEY_W;  // VK_W
        case 0x58: return KEY_X;  // VK_X
        case 0x59: return KEY_Y;  // VK_Y
        case 0x5A: return KEY_Z;  // VK_Z

        // Numbers
        case 0x30: return KEY_0;  // VK_0
        case 0x31: return KEY_1;  // VK_1
        case 0x32: return KEY_2;  // VK_2
        case 0x33: return KEY_3;  // VK_3
        case 0x34: return KEY_4;  // VK_4
        case 0x35: return KEY_5;  // VK_5
        case 0x36: return KEY_6;  // VK_6
        case 0x37: return KEY_7;  // VK_7
        case 0x38: return KEY_8;  // VK_8
        case 0x39: return KEY_9;  // VK_9

        // Function keys
        case 0x70: return KEY_F1;   // VK_F1
        case 0x71: return KEY_F2;   // VK_F2
        case 0x72: return KEY_F3;   // VK_F3
        case 0x73: return KEY_F4;   // VK_F4
        case 0x74: return KEY_F5;   // VK_F5
        case 0x75: return KEY_F6;   // VK_F6
        case 0x76: return KEY_F7;   // VK_F7
        case 0x77: return KEY_F8;   // VK_F8
        case 0x78: return KEY_F9;   // VK_F9
        case 0x79: return KEY_F10;  // VK_F10
        case 0x7A: return KEY_F11;  // VK_F11
        case 0x7B: return KEY_F12;  // VK_F12

        // Special keys
        case 0x08: return KEY_BACKSPACE;  // VK_BACK
        case 0x09: return KEY_TAB;        // VK_TAB
        case 0x0D: return KEY_ENTER;      // VK_RETURN
        case 0x10: return KEY_LEFTSHIFT;  // VK_SHIFT
        case 0x11: return KEY_LEFTCTRL;   // VK_CONTROL
        case 0x12: return KEY_LEFTALT;    // VK_MENU
        case 0x1B: return KEY_ESC;        // VK_ESCAPE
        case 0x20: return KEY_SPACE;      // VK_SPACE

        // Arrow keys
        case 0x25: return KEY_LEFT;   // VK_LEFT
        case 0x26: return KEY_UP;     // VK_UP
        case 0x27: return KEY_RIGHT;  // VK_RIGHT
        case 0x28: return KEY_DOWN;   // VK_DOWN

        // Home cluster
        case 0x2D: return KEY_INSERT;   // VK_INSERT
        case 0x2E: return KEY_DELETE;   // VK_DELETE
        case 0x24: return KEY_HOME;     // VK_HOME
        case 0x23: return KEY_END;      // VK_END
        case 0x21: return KEY_PAGEUP;   // VK_PRIOR
        case 0x22: return KEY_PAGEDOWN; // VK_NEXT

        default:
            qWarning() << "Unmapped Windows VK code:" << QString::number(vkCode, 16);
            return KEY_RESERVED; // Unknown key
    }
}

void KeyboardInjector::pressKey(int qtKeyCode)
{
    getKeyboardDevice(); // Ensure device is created
    if (!s_keyboardDevice) return;

    int linuxKey = windowsVKToLinuxKey(qtKeyCode);
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
        int linuxKey = windowsVKToLinuxKey(qtKeyCode);
        if (linuxKey != KEY_RESERVED) {
            libevdev_uinput_write_event(s_keyboardDevice.get(), EV_KEY, linuxKey, 1);
        }
    }
    libevdev_uinput_write_event(s_keyboardDevice.get(), EV_SYN, SYN_REPORT, 0);

    // Wait
    QThread::msleep(PRESS_INTERVAL);

    // Release all keys
    for (int qtKeyCode : qtKeys) {
        int linuxKey = windowsVKToLinuxKey(qtKeyCode);
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

    int linuxKey = windowsVKToLinuxKey(qtKeyCode);
    if (linuxKey == KEY_RESERVED) return;

    libevdev_uinput_write_event(s_keyboardDevice.get(), EV_KEY, linuxKey, 1);
    libevdev_uinput_write_event(s_keyboardDevice.get(), EV_SYN, SYN_REPORT, 0);
}

void KeyboardInjector::keyUp(int qtKeyCode)
{
    getKeyboardDevice(); // Ensure device is created
    if (!s_keyboardDevice) return;

    int linuxKey = windowsVKToLinuxKey(qtKeyCode);
    if (linuxKey == KEY_RESERVED) return;

    libevdev_uinput_write_event(s_keyboardDevice.get(), EV_KEY, linuxKey, 0);
    libevdev_uinput_write_event(s_keyboardDevice.get(), EV_SYN, SYN_REPORT, 0);
}

void KeyboardInjector::keyComboUp(std::vector<int> qtKeys)
{
    getKeyboardDevice(); // Ensure device is created
    if (!s_keyboardDevice) return;

    for (int qtKeyCode : qtKeys) {
        int linuxKey = windowsVKToLinuxKey(qtKeyCode);
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
        int linuxKey = windowsVKToLinuxKey(qtKeyCode);
        if (linuxKey != KEY_RESERVED) {
            libevdev_uinput_write_event(s_keyboardDevice.get(), EV_KEY, linuxKey, 1);
        }
    }
    libevdev_uinput_write_event(s_keyboardDevice.get(), EV_SYN, SYN_REPORT, 0);
}

void KeyboardInjector::typeUnicodeString(const QString& str)
{
    // For simplicity, we'll just type ASCII characters by converting QString to std::string
    // Full Unicode support would require more complex input method integration
    std::string stdStr = str.toStdString();
    getKeyboardDevice(); // Ensure device is created
    if (!s_keyboardDevice) return;

    for (char c : stdStr) {
        if (c >= 'a' && c <= 'z') {
            // Lowercase letters
            int qtKeyCode = 0x41 + (c - 'a'); // Convert to VK_A + offset
            pressKey(qtKeyCode);
        } else if (c >= 'A' && c <= 'Z') {
            // Uppercase letters - press shift + letter
            std::vector<int> combo = {0x10, 0x41 + (c - 'A')}; // VK_SHIFT + VK_A + offset
            pressKeyCombo(combo);
        } else if (c >= '0' && c <= '9') {
            // Numbers
            int qtKeyCode = 0x30 + (c - '0'); // VK_0 + offset
            pressKey(qtKeyCode);
        } else if (c == ' ') {
            pressKey(0x20); // VK_SPACE
        }
        // Add more character mappings as needed
    }
}
