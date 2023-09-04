#include "preferences.h"
#include "settings_key_variables.h"
#include "settings.h"
#include "ui_preferences.h"
#include <QDebug>
#include <QSlider>
#include <QPixmap>
#include <QLabel>
#include <QLayout>
#include <QMouseEvent>
#include <QKeyEvent>
#include "winuser.h"
#include "windows.h"


Preferences::Preferences(QWidget *parent) : QDialog(parent), ui(new Ui::Preferences)
{
	ui->setupUi(this);
    install_event_filter();
	ui->horizontalSlider->adjustSize();
    ui->buttonBox->connect(ui->buttonBox, &QDialogButtonBox::accepted, [=] {    // running the functions to change and save the new settings if the user presses ok
		this->change_mouse_sensitivity(ui->horizontalSlider->value() * 100);
        qDebug() << mouse_sensivity;
        save_setting(setting_keys::Mouse_sensivity, mouse_sensivity / 100); //saving the new mouse sensivity
        change_key_inputs(); //changing and saving key maps
	});
    ui->formLayout->setSizeConstraint(QLayout::SetMinimumSize);
    ui->formLayout->setHorizontalSpacing(50);
    ui->formLayout->setVerticalSpacing(10);
	ui->horizontalSlider->setValue(mouse_sensivity / 100);
    Preferences::load_keys();
    QList<QLineEdit*> lst = ui->KeyMaps->findChildren<QLineEdit*>();
    qDebug() << lst.size();
    for (QList<QLineEdit*>::iterator ptr = lst.begin(); ptr != lst.end();++ptr) {
        qDebug() << (*ptr)->objectName();
    }
}

/**
 * @brief Preferences::change_mouse_sensitivity
 * changes the mouse sensivity or the cursor speed
 * @param value
 * the amount of mouse sensivity you want to set.
 */
void Preferences::change_mouse_sensitivity(int value)
{
	mouse_sensivity = value;
}

/**
 * @brief Preferences::change_key_inputs
 * this changes the keyboard maps and saves those changes in the config file.
 * this function is executed if the user presses ok button.
 */

void Preferences::change_key_inputs()
{
    //change and save key maps
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_X] = this->temp[0];
    save_setting(keymaps[setting_keys::keys::X], this->temp[0]);
    /*------------------------------------------------------------*/
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_Y] = this->temp[1];
    save_setting(keymaps[setting_keys::keys::Y], this->temp[1]);
    /*------------------------------------------------------------*/
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_A] = this->temp[2];
    save_setting(keymaps[setting_keys::keys::A], this->temp[2]);
    /*------------------------------------------------------------*/
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_B] = this->temp[3];
    save_setting(keymaps[setting_keys::keys::B], this->temp[3]);
    /*------------------------------------------------------------*/
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_LeftThumbstick] = this->temp[4];
    save_setting(keymaps[setting_keys::keys::LT], this->temp[4]);
    /*------------------------------------------------------------*/
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_RightThumbstick] = this->temp[5];
    save_setting(keymaps[setting_keys::keys::RT], this->temp[5]);
    /*------------------------------------------------------------*/
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_DPadDown] = this->temp[6];
    save_setting(keymaps[setting_keys::keys::DPADDOWN], this->temp[6]);
    /*------------------------------------------------------------*/
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_DPadUp] = this->temp[7];
    save_setting(keymaps[setting_keys::keys::DPADUP], this->temp[7]);
    /*------------------------------------------------------------*/
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_DPadRight] = this->temp[8];
    save_setting(keymaps[setting_keys::keys::DPADRIGHT], this->temp[8]);
    /*------------------------------------------------------------*/
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_DPadLeft] = this->temp[9];
    save_setting(keymaps[setting_keys::keys::DPADLEFT], this->temp[9]);
}

/**
 * @brief Preferences::get_scan_code
 * gets the name of the of corrosponding key or virutal key code.
 * @param vk
 * the virtual key code of the key you want to get.
 * @param a
 * the buffer to store the name of the key.
 * @param size
 * size of the buffer in which the name is stored to ensure memory safety
 */
void Preferences::get_scan_code(WORD vk, char* a, int size)
{
    char sc = MapVirtualKeyA((UINT)vk, MAPVK_VK_TO_CHAR);
    if(sc >= '0' && sc <= 'Z') {
        strncpy(a, "", size);
        strncpy(a, &sc, 1);
    }
    else {
        strncpy(a, vk_maps[vk], size);
    }
}

/**
 * @brief Preferences::load_keys
 * displays the key to which each button is mapped to.
 * saves the initial key maps in variables that can be changed later if user wants to.
 */
void Preferences::load_keys()
{
    char buffer[256];
    this->temp[0] = GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_X];
    get_scan_code(GAMEPAD_BUTTONS[GamepadButtons_X], buffer, 256);
    this->ui->xmap->setText(QString(buffer));
    /*------------------------------------------------------------*/
    this->temp[1] = GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_Y];
    get_scan_code(GAMEPAD_BUTTONS[GamepadButtons_Y], buffer, 256);
    this->ui->ymap->setText(QString(buffer));
    /*------------------------------------------------------------*/
    this->temp[2] = GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_A];
    get_scan_code(GAMEPAD_BUTTONS[GamepadButtons_A], buffer, 256);
    this->ui->amap->setText(QString(buffer));
    /*------------------------------------------------------------*/
    this->temp[3] = GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_B];
    get_scan_code(GAMEPAD_BUTTONS[GamepadButtons_B], buffer, 256);
    this->ui->bmap->setText(QString(buffer));
    /*------------------------------------------------------------*/
    this->temp[4] = GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_RightThumbstick];
    get_scan_code(GAMEPAD_BUTTONS[GamepadButtons_RightThumbstick], buffer, 256);
    this->ui->Rtmap->setText(QString(buffer));
    /*------------------------------------------------------------*/
    this->temp[5] = GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_LeftThumbstick];
    get_scan_code(GAMEPAD_BUTTONS[GamepadButtons_LeftThumbstick], buffer, 256);
    this->ui->Ltmap->setText(QString(buffer));
    /*------------------------------------------------------------*/
    this->temp[6] = GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_DPadDown];
    get_scan_code(GAMEPAD_BUTTONS[GamepadButtons_DPadDown], buffer, 256);
    this->ui->ddownmap->setText(QString(buffer));
    /*------------------------------------------------------------*/
    this->temp[7] = GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_DPadUp];
    get_scan_code(GAMEPAD_BUTTONS[GamepadButtons_DPadUp], buffer, 256);
    this->ui->dupmap->setText(QString(buffer));
    /*------------------------------------------------------------*/
    this->temp[8] = GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_DPadRight];
    get_scan_code(GAMEPAD_BUTTONS[GamepadButtons_DPadRight], buffer, 256);
    this->ui->drightmap->setText(QString(buffer));
    /*------------------------------------------------------------*/
    this->temp[9] = GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_DPadLeft];
    get_scan_code(GAMEPAD_BUTTONS[GamepadButtons_DPadLeft], buffer, 256);
    this->ui->dleftmap->setText(QString(buffer));
}

/**
 * @brief Preferences::eventFilter
 * the event filter virtual function is redefined to to filter for mouse and keyboard inputs when user tries to change the button-key maps.
 * checks which object is sending the event and type of event.
 * if event is a keyboard or mousebutton press than map and the object is buttonmap than get the virtual key code of the key pressed and store the change in a temporary variable.
 * @param sender
 * to get the address of the object that is triggering the event.
 * @param event
 * to capture the event that was triggered.
 * @return [bool]
 * true if event is handled else false.
 */
bool Preferences::eventFilter(QObject *sender, QEvent *event)
{
    QList<QLineEdit*> lst = ui->KeyMaps->findChildren<QLineEdit*>();
    for(int i=0;i<lst.size();i++) {
        if(sender == lst[i]) {
            QLineEdit *map = static_cast<QLineEdit*>(sender);
            if(event->type() == QEvent::KeyRelease && map->text() == "") {
                QKeyEvent* key_press = (QKeyEvent*)event;
                this->temp[i] = key_press->nativeVirtualKey();
                char buffer[256];
                get_scan_code(key_press->nativeVirtualKey(), buffer, 256);
                map->setText(QString(buffer));
            }
            else if(event->type() == QEvent::MouseButtonPress && map->text() == "") {
                QMouseEvent* mouse_press = static_cast<QMouseEvent*>(event);
                char buffer[256];
                bool valid = true;
                UINT button = mouse_press->button();
                switch(button) {
                case Qt::MouseButton::LeftButton:
                    this->temp[i] = VK_LBUTTON;
                    get_scan_code(VK_LBUTTON, buffer, 256);
                    break;
                case Qt::MouseButton::RightButton:
                    this->temp[i] = VK_RBUTTON;
                    get_scan_code(VK_RBUTTON, buffer, 256);
                    break;
                case Qt::MouseButton::MiddleButton:
                    this->temp[i] = VK_MBUTTON;
                    get_scan_code(VK_MBUTTON, buffer, 256);
                    break;
                default:
                    qDebug() << "Some Error Occured No Legal Mouse Button found";
                    valid = false;
                }
                if(valid)
                map->setText(QString(buffer));
            }
        }
    }
    return QWidget::eventFilter(sender,event);
}

/**
 * @brief Preferences::keyPressEvent
 * make the Qdialog box ignores the enter key and escape key presses when the focus is on button map
 * @param e
 * capture the key_press event
 */
void Preferences::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return || e->key() == Qt::Key_Escape)
        return;
    return QDialog::keyPressEvent(e);
}

/**
 * @brief Preferences::install_event_filter
 * install the above event filter on all the button maps to capture the key presses when they have the focus.
 */
void Preferences::install_event_filter() {
    QList<QLineEdit*> lst = ui->KeyMaps->findChildren<QLineEdit*>();
    for(QList<QLineEdit*>::iterator ptr = lst.begin();ptr != lst.end(); ++ptr) {
        (*ptr)->installEventFilter(this);
    }
}

Preferences::~Preferences()
{
    delete ui;
}
