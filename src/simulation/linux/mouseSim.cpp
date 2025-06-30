#include "../mouseSim.hpp"

#include <QDebug>
#include <QThread>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

// Static member initialization
std::unique_ptr<libevdev_uinput, void(*)(libevdev_uinput*)> MouseInjector::s_mouseDevice(nullptr, libevdev_uinput_destroy);

std::unique_ptr<libevdev_uinput, void(*)(libevdev_uinput*)> MouseInjector::getMouseDevice()
{
    if (s_mouseDevice) {
        return std::unique_ptr<libevdev_uinput, void(*)(libevdev_uinput*)>(nullptr, libevdev_uinput_destroy);
    }

    // Create a new libevdev device for mouse
    libevdev* dev = libevdev_new();
    if (!dev) {
        qCritical() << "Failed to create mouse libevdev device";
        return std::unique_ptr<libevdev_uinput, void(*)(libevdev_uinput*)>(nullptr, libevdev_uinput_destroy);
    }

    // Set device properties
    libevdev_set_name(dev, "Virtual Gamepad PC Mouse");
    libevdev_set_id_bustype(dev, BUS_USB);
    libevdev_set_id_vendor(dev, 0x1d6b);  // Linux Foundation
    libevdev_set_id_product(dev, 0x0002); // Generic mouse
    libevdev_set_id_version(dev, 0x0100);

    // Enable mouse events
    libevdev_enable_event_type(dev, EV_KEY);
    libevdev_enable_event_type(dev, EV_REL);

    // Enable mouse buttons
    libevdev_enable_event_code(dev, EV_KEY, BTN_LEFT, nullptr);
    libevdev_enable_event_code(dev, EV_KEY, BTN_RIGHT, nullptr);
    libevdev_enable_event_code(dev, EV_KEY, BTN_MIDDLE, nullptr);

    // Enable mouse movement
    libevdev_enable_event_code(dev, EV_REL, REL_X, nullptr);
    libevdev_enable_event_code(dev, EV_REL, REL_Y, nullptr);
    libevdev_enable_event_code(dev, EV_REL, REL_WHEEL, nullptr);
    libevdev_enable_event_code(dev, EV_REL, REL_HWHEEL, nullptr);

    // Create uinput device
    libevdev_uinput* uidev;
    int ret = libevdev_uinput_create_from_device(dev, LIBEVDEV_UINPUT_OPEN_MANAGED, &uidev);
    libevdev_free(dev);

    if (ret < 0) {
        qCritical() << "Failed to create mouse uinput device:" << strerror(-ret);
        return std::unique_ptr<libevdev_uinput, void(*)(libevdev_uinput*)>(nullptr, libevdev_uinput_destroy);
    }

    s_mouseDevice.reset(uidev);
    qDebug() << "Virtual mouse device created successfully";
    
    return std::unique_ptr<libevdev_uinput, void(*)(libevdev_uinput*)>(nullptr, libevdev_uinput_destroy);
}

void MouseInjector::moveMouseToPosition(int x, int y)
{
    // On Linux, we can't directly set absolute position with uinput mouse devices
    // This would require a different approach or additional setup
    // For now, we'll implement this as a relative move
    // TODO: Implement absolute positioning if needed
    qWarning() << "Absolute mouse positioning not implemented on Linux. Use moveMouseByOffset instead.";
}

void MouseInjector::moveMouseByOffset(int x, int y)
{
    getMouseDevice(); // Ensure device is created
    if (!s_mouseDevice) return;

    if (x != 0) {
        libevdev_uinput_write_event(s_mouseDevice.get(), EV_REL, REL_X, x);
    }
    if (y != 0) {
        libevdev_uinput_write_event(s_mouseDevice.get(), EV_REL, REL_Y, y);
    }
    
    if (x != 0 || y != 0) {
        libevdev_uinput_write_event(s_mouseDevice.get(), EV_SYN, SYN_REPORT, 0);
    }
}

void MouseInjector::singleClick()
{
    leftClick();
    // On Linux, we don't need to worry about double-click timing as much
    // since the window manager handles that
}

void MouseInjector::doubleClick()
{
    leftClick();
    QThread::msleep(50); // Small delay between clicks
    leftClick();
}

void MouseInjector::leftClick()
{
    getMouseDevice(); // Ensure device is created
    if (!s_mouseDevice) return;

    // Press
    libevdev_uinput_write_event(s_mouseDevice.get(), EV_KEY, BTN_LEFT, 1);
    libevdev_uinput_write_event(s_mouseDevice.get(), EV_SYN, SYN_REPORT, 0);

    QThread::msleep(ClickHoldTime);

    // Release
    libevdev_uinput_write_event(s_mouseDevice.get(), EV_KEY, BTN_LEFT, 0);
    libevdev_uinput_write_event(s_mouseDevice.get(), EV_SYN, SYN_REPORT, 0);
}

void MouseInjector::rightClick()
{
    getMouseDevice(); // Ensure device is created
    if (!s_mouseDevice) return;

    // Press
    libevdev_uinput_write_event(s_mouseDevice.get(), EV_KEY, BTN_RIGHT, 1);
    libevdev_uinput_write_event(s_mouseDevice.get(), EV_SYN, SYN_REPORT, 0);

    QThread::msleep(ClickHoldTime);

    // Release
    libevdev_uinput_write_event(s_mouseDevice.get(), EV_KEY, BTN_RIGHT, 0);
    libevdev_uinput_write_event(s_mouseDevice.get(), EV_SYN, SYN_REPORT, 0);
}

void MouseInjector::middleClick()
{
    getMouseDevice(); // Ensure device is created
    if (!s_mouseDevice) return;

    // Press
    libevdev_uinput_write_event(s_mouseDevice.get(), EV_KEY, BTN_MIDDLE, 1);
    libevdev_uinput_write_event(s_mouseDevice.get(), EV_SYN, SYN_REPORT, 0);

    QThread::msleep(ClickHoldTime);

    // Release
    libevdev_uinput_write_event(s_mouseDevice.get(), EV_KEY, BTN_MIDDLE, 0);
    libevdev_uinput_write_event(s_mouseDevice.get(), EV_SYN, SYN_REPORT, 0);
}

void MouseInjector::leftDown()
{
    getMouseDevice(); // Ensure device is created
    if (!s_mouseDevice) return;

    libevdev_uinput_write_event(s_mouseDevice.get(), EV_KEY, BTN_LEFT, 1);
    libevdev_uinput_write_event(s_mouseDevice.get(), EV_SYN, SYN_REPORT, 0);
}

void MouseInjector::leftUp()
{
    getMouseDevice(); // Ensure device is created
    if (!s_mouseDevice) return;

    libevdev_uinput_write_event(s_mouseDevice.get(), EV_KEY, BTN_LEFT, 0);
    libevdev_uinput_write_event(s_mouseDevice.get(), EV_SYN, SYN_REPORT, 0);
}

void MouseInjector::rightDown()
{
    getMouseDevice(); // Ensure device is created
    if (!s_mouseDevice) return;

    libevdev_uinput_write_event(s_mouseDevice.get(), EV_KEY, BTN_RIGHT, 1);
    libevdev_uinput_write_event(s_mouseDevice.get(), EV_SYN, SYN_REPORT, 0);
}

void MouseInjector::rightUp()
{
    getMouseDevice(); // Ensure device is created
    if (!s_mouseDevice) return;

    libevdev_uinput_write_event(s_mouseDevice.get(), EV_KEY, BTN_RIGHT, 0);
    libevdev_uinput_write_event(s_mouseDevice.get(), EV_SYN, SYN_REPORT, 0);
}

void MouseInjector::middleDown()
{
    getMouseDevice(); // Ensure device is created
    if (!s_mouseDevice) return;

    libevdev_uinput_write_event(s_mouseDevice.get(), EV_KEY, BTN_MIDDLE, 1);
    libevdev_uinput_write_event(s_mouseDevice.get(), EV_SYN, SYN_REPORT, 0);
}

void MouseInjector::middleUp()
{
    getMouseDevice(); // Ensure device is created
    if (!s_mouseDevice) return;

    libevdev_uinput_write_event(s_mouseDevice.get(), EV_KEY, BTN_MIDDLE, 0);
    libevdev_uinput_write_event(s_mouseDevice.get(), EV_SYN, SYN_REPORT, 0);
}

void MouseInjector::scrollUp()
{
    getMouseDevice(); // Ensure device is created
    if (!s_mouseDevice) return;

    libevdev_uinput_write_event(s_mouseDevice.get(), EV_REL, REL_WHEEL, 1);
    libevdev_uinput_write_event(s_mouseDevice.get(), EV_SYN, SYN_REPORT, 0);
}

void MouseInjector::scrollDown()
{
    getMouseDevice(); // Ensure device is created
    if (!s_mouseDevice) return;

    libevdev_uinput_write_event(s_mouseDevice.get(), EV_REL, REL_WHEEL, -1);
    libevdev_uinput_write_event(s_mouseDevice.get(), EV_SYN, SYN_REPORT, 0);
}
