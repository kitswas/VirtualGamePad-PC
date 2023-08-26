#include "settings_key_variables.h"

int mouse_sensivity = 1000;
int port = 7878;
std::map<GamepadButtons, UINT> GAMEPAD_BUTTONS = {{GamepadButtons::GamepadButtons_Menu, VK_MENU},
                                                  {GamepadButtons::GamepadButtons_View, VK_TAB},
                                                  {GamepadButtons::GamepadButtons_A, VK_RETURN},
                                                  {GamepadButtons::GamepadButtons_B, 'B'},
                                                  {GamepadButtons::GamepadButtons_X, VK_SHIFT},
                                                  {GamepadButtons::GamepadButtons_Y, VK_CONTROL},
                                                  {GamepadButtons::GamepadButtons_DPadUp, VK_UP},
                                                  {GamepadButtons::GamepadButtons_DPadDown, VK_DOWN},
                                                  {GamepadButtons::GamepadButtons_DPadLeft, VK_LEFT},
                                                  {GamepadButtons::GamepadButtons_DPadRight, VK_RIGHT},
//                                                  {GamepadButtons::GamepadButtons_LeftShoulder, 'E'},
//                                                  {GamepadButtons::GamepadButtons_RightShoulder, VK_SPACE},
                                                  {GamepadButtons::GamepadButtons_LeftThumbstick, VK_LBUTTON},
                                                  {GamepadButtons::GamepadButtons_RightThumbstick, VK_NEXT}};

std::map<Thumbstick, WORD> THUMBSTICK_KEYS = {
    {Thumbstick::LeftThumbstickUp, 'W'},		{Thumbstick::LeftThumbstickDown, 'S'},
    {Thumbstick::LeftThumbstickLeft, 'A'},		{Thumbstick::LeftThumbstickRight, 'D'},
    {Thumbstick::RightThumbstickUp, VK_UP},		{Thumbstick::RightThumbstickDown, VK_DOWN},
    {Thumbstick::RightThumbstickLeft, VK_LEFT}, {Thumbstick::RightThumbstickRight, VK_RIGHT}};

std::map<UINT, const char*> vk_maps = {
    {VK_LBUTTON, "LMButton"},
    {VK_RBUTTON, "RMButton"},
    {VK_MBUTTON, "MMButton"},
    {VK_BACK, "BACKSPACE"},
    {VK_TAB, "TAB"},
    {VK_RETURN, "ENTER"},
    {VK_SHIFT, "SHIFT"},
    {VK_CONTROL, "CTRL"},
    {VK_CAPITAL, "CAPITAL"},
    {VK_ESCAPE, "ESCAPE"},
    {VK_SPACE, "SPACE"},
    {VK_PRIOR, "PageUP"},
    {VK_NEXT, "PageDOWN"},
    {VK_END, "END"},
    {VK_HOME, "HOME"},
    {VK_LEFT, "LEFT"},
    {VK_UP, "UP"},
    {VK_RIGHT, "RIGHT"},
    {VK_DOWN, "DOWN"},
    {VK_INSERT, "INS"},
    {VK_DELETE, "DEL"},
    {VK_OEM_PERIOD, "."},
    {VK_OEM_COMMA, ","},
    {VK_OEM_MINUS, "-"}
};
