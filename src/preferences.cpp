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
	ui->buttonBox->connect(ui->buttonBox, &QDialogButtonBox::accepted, [=] {
		this->change_mouse_sensitivity(ui->horizontalSlider->value() * 100);
		qDebug() << mouse_sensivity << Qt::endl;
		save_setting(setting_keys::Mouse_sensivity, mouse_sensivity / 100);
        change_key_inputs(); //saving key maps
	});
    ui->formLayout->setSizeConstraint(QLayout::SetMinimumSize);
    Preferences::set_button_icons();
    ui->formLayout->setHorizontalSpacing(50);
    ui->formLayout->setVerticalSpacing(10);
	ui->horizontalSlider->setValue(mouse_sensivity / 100);
    Preferences::load_keys();
}

void Preferences::set_button_icons()
{
    QPixmap x;
    x.load("C:\\Users\\saisi\\Memory_card\\VirtualGamePad-PC\\res\\logos\\icons8-xbox-x-30.png");           // https://icons8.com/icon/80114/xbox-x
    ui->xbutton->setPixmap(x);
    QPixmap y;
    y.load("C:\\Users\\saisi\\Memory_card\\VirtualGamePad-PC\\res\\logos\\icons8-xbox-y-30.png");           // https://icons8.com/icon/80111/xbox-y
    ui->ybutton->setPixmap(y);
    QPixmap a;
    a.load("C:\\Users\\saisi\\Memory_card\\VirtualGamePad-PC\\res\\logos\\icons8-xbox-a-30.png");           // https://icons8.com/icon/80108/xbox-a
    ui->abutton->setPixmap(a);
    QPixmap b;
    b.load("C:\\Users\\saisi\\Memory_card\\VirtualGamePad-PC\\res\\logos\\icons8-xbox-b-30.png");           // https://icons8.com/icon/80102/xbox-b
    ui->bbutton->setPixmap(b);
    QPixmap rt;
    rt.load("C:\\Users\\saisi\\Memory_card\\VirtualGamePad-PC\\res\\logos\\icons8-xbox-rt-30.png");         // https://icons8.com/icon/80116/xbox-rt
    ui->Rt->setPixmap(rt);
    QPixmap lt;
    lt.load("C:\\Users\\saisi\\Memory_card\\VirtualGamePad-PC\\res\\logos\\icons8-xbox-lt-30.png");         // https://icons8.com/icon/80107/xbox-lt
    ui->Lt->setPixmap(lt);
    QPixmap dpad_down;
    dpad_down.load("C:\\Users\\saisi\\Memory_card\\VirtualGamePad-PC\\res\\logos\\icons8-down-30.png");     // https://icons8.com/
    ui->ddown->setPixmap(dpad_down);
    QPixmap dpad_up;
    dpad_up.load("C:\\Users\\saisi\\Memory_card\\VirtualGamePad-PC\\res\\logos\\icons8-up-30.png");         // https://icons8.com/icon/100000/up
    ui->dup->setPixmap(dpad_up);
    QPixmap dpad_right;
    dpad_right.load("C:\\Users\\saisi\\Memory_card\\VirtualGamePad-PC\\res\\logos\\icons8-right-30.png");   // https://icons8.com/icon/98968/right
    ui->dright->setPixmap(dpad_right);
    QPixmap dpad_left;
    dpad_left.load("C:\\Users\\saisi\\Memory_card\\VirtualGamePad-PC\\res\\logos\\icons8-left-30.png");     // https://icons8.com/icon/98961/left
    ui->dleft->setPixmap(dpad_left);
}

int Preferences::change_mouse_sensitivity(int value)
{
	mouse_sensivity = value;
    return 1;
}

void Preferences::change_key_inputs()
{
    //change and save key maps
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_X] = this->X;
    save_setting(keymaps[setting_keys::keys::X], this->X);
    /*------------------------------------------------------------*/
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_Y] = this->Y;
    save_setting(keymaps[setting_keys::keys::Y], this->Y);
    /*------------------------------------------------------------*/
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_A] = this->A;
    save_setting(keymaps[setting_keys::keys::A], this->A);
    /*------------------------------------------------------------*/
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_B] = this->B;
    save_setting(keymaps[setting_keys::keys::B], this->B);
    /*------------------------------------------------------------*/
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_LeftThumbstick] = this->LT;
    save_setting(keymaps[setting_keys::keys::LT], this->LT);
    /*------------------------------------------------------------*/
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_RightThumbstick] = this->RT;
    save_setting(keymaps[setting_keys::keys::RT], this->RT);
    /*------------------------------------------------------------*/
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_DPadDown] = this->DDOWN;
    save_setting(keymaps[setting_keys::keys::DPADDOWN], this->DDOWN);
    /*------------------------------------------------------------*/
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_DPadUp] = this->DUP;
    save_setting(keymaps[setting_keys::keys::DPADUP], this->DUP);
    /*------------------------------------------------------------*/
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_DPadRight] = this->DRIGHT;
    save_setting(keymaps[setting_keys::keys::DPADRIGHT], this->DRIGHT);
    /*------------------------------------------------------------*/
    GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_DPadLeft] = this->DLEFT;
    save_setting(keymaps[setting_keys::keys::DPADLEFT], this->DLEFT);
}

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

void Preferences::load_keys()
{
    char buffer[256];
    this->X = GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_X];
    get_scan_code(GAMEPAD_BUTTONS[GamepadButtons_X], buffer, 256);
    this->ui->xmap->setText(QString(buffer));
    /*------------------------------------------------------------*/
    this->Y = GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_Y];
    get_scan_code(GAMEPAD_BUTTONS[GamepadButtons_Y], buffer, 256);
    this->ui->ymap->setText(QString(buffer));
    /*------------------------------------------------------------*/
    this->A = GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_A];
    get_scan_code(GAMEPAD_BUTTONS[GamepadButtons_A], buffer, 256);
    this->ui->amap->setText(QString(buffer));
    /*------------------------------------------------------------*/
    this->B = GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_B];
    get_scan_code(GAMEPAD_BUTTONS[GamepadButtons_B], buffer, 256);
    this->ui->bmap->setText(QString(buffer));
    /*------------------------------------------------------------*/
    this->RT = GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_RightThumbstick];
    get_scan_code(GAMEPAD_BUTTONS[GamepadButtons_RightThumbstick], buffer, 256);
    this->ui->Rtmap->setText(QString(buffer));
    /*------------------------------------------------------------*/
    this->LT = GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_LeftThumbstick];
    get_scan_code(GAMEPAD_BUTTONS[GamepadButtons_LeftThumbstick], buffer, 256);
    this->ui->Ltmap->setText(QString(buffer));
    /*------------------------------------------------------------*/
    this->DDOWN = GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_DPadDown];
    get_scan_code(GAMEPAD_BUTTONS[GamepadButtons_DPadDown], buffer, 256);
    this->ui->ddownmap->setText(QString(buffer));
    /*------------------------------------------------------------*/
    this->DUP = GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_DPadUp];
    get_scan_code(GAMEPAD_BUTTONS[GamepadButtons_DPadUp], buffer, 256);
    this->ui->dupmap->setText(QString(buffer));
    /*------------------------------------------------------------*/
    this->DRIGHT = GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_DPadRight];
    get_scan_code(GAMEPAD_BUTTONS[GamepadButtons_DPadRight], buffer, 256);
    this->ui->drightmap->setText(QString(buffer));
    /*------------------------------------------------------------*/
    this->DLEFT = GAMEPAD_BUTTONS[GamepadButtons::GamepadButtons_DPadLeft];
    get_scan_code(GAMEPAD_BUTTONS[GamepadButtons_DPadLeft], buffer, 256);
    this->ui->dleftmap->setText(QString(buffer));
}

bool Preferences::eventFilter(QObject *sender, QEvent *event)
{
    if(sender == ui->xmap) {
        if(event->type() == QEvent::KeyRelease) {
            QKeyEvent* key_press = (QKeyEvent*)event;
            this->X = key_press->nativeVirtualKey();
            char buffer[256];
            get_scan_code(key_press->nativeVirtualKey(), buffer, 256);
            ui->xmap->setText(QString(buffer));
        }
        else if(event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent* mouse_press = static_cast<QMouseEvent*>(event);
            char buffer[256];
            bool valid = true;
            UINT button = mouse_press->button();
            switch(button) {
            case Qt::MouseButton::LeftButton:
                this->X = VK_LBUTTON;
                get_scan_code(VK_LBUTTON, buffer, 256);
                break;
            case Qt::MouseButton::RightButton:
                this->X = VK_RBUTTON;
                get_scan_code(VK_RBUTTON, buffer, 256);
                break;
            case Qt::MouseButton::MiddleButton:
                this->X = VK_MBUTTON;
                get_scan_code(VK_MBUTTON, buffer, 256);
                break;
            default:
                qDebug() << "Some Error Occured No Legal Mouse Button found" << Qt::endl;
                valid = false;
            }
            if(valid)
            ui->xmap->setText(QString(buffer));
        }
    }
    else if(sender == ui->ymap) {
        if(event->type() == QEvent::KeyRelease) {
            QKeyEvent* key_press = (QKeyEvent*)event;
            this->Y = key_press->nativeVirtualKey();
            char buffer[256];
            get_scan_code(key_press->nativeVirtualKey(), buffer, 256);
            ui->ymap->setText(QString(buffer));
        }
        else if(event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent* mouse_press = static_cast<QMouseEvent*>(event);
            char buffer[256];
            bool valid = true;
            UINT button = mouse_press->button();
            switch(button) {
            case Qt::MouseButton::LeftButton:
                this->Y = VK_LBUTTON;
                get_scan_code(VK_LBUTTON, buffer, 256);
                break;
            case Qt::MouseButton::RightButton:
                this->Y = VK_RBUTTON;
                get_scan_code(VK_RBUTTON, buffer, 256);
                break;
            case Qt::MouseButton::MiddleButton:
                this->Y = VK_MBUTTON;
                get_scan_code(VK_MBUTTON, buffer, 256);
                break;
            default:
                qDebug() << "Some Error Occured No Legal Mouse Button found" << Qt::endl;
                valid = false;
            }
            if(valid)
            ui->ymap->setText(QString(buffer));
        }
    }
    else if(sender == ui->amap) {
        if(event->type() == QEvent::KeyRelease) {
            QKeyEvent* key_press = static_cast<QKeyEvent*>(event);
            this->A = key_press->nativeVirtualKey();
            char buffer[256];
            get_scan_code(key_press->nativeVirtualKey(), buffer, 256);
            ui->amap->setText(buffer);
        }
        else if(event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent* mouse_press = static_cast<QMouseEvent*>(event);
            char buffer[256];
            bool valid = true;
            UINT button = mouse_press->button();
            switch(button) {
            case Qt::MouseButton::LeftButton:
                this->A = VK_LBUTTON;
                get_scan_code(VK_LBUTTON, buffer, 256);
                break;
            case Qt::MouseButton::RightButton:
                this->A = VK_RBUTTON;
                get_scan_code(VK_RBUTTON, buffer, 256);
                break;
            case Qt::MouseButton::MiddleButton:
                this->A = VK_MBUTTON;
                get_scan_code(VK_MBUTTON, buffer, 256);
                break;
            default:
                qDebug() << "Some Error Occured No Legal Mouse Button found" << Qt::endl;
                valid = false;
            }
            if(valid)
            ui->amap->setText(QString(buffer));
        }
    }
    else if(sender == ui->bmap) {
        if(event->type() == QEvent::KeyRelease) {
            QKeyEvent* key_press = static_cast<QKeyEvent*>(event);
            this->B = key_press->nativeVirtualKey();
            char buffer[256];
            get_scan_code(key_press->nativeVirtualKey(), buffer, 256);
            ui->bmap->setText(QString(buffer));
        }
        else if(event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent* mouse_press = static_cast<QMouseEvent*>(event);
            char buffer[256];
            bool valid = true;
            UINT button = mouse_press->button();
            switch(button) {
            case Qt::MouseButton::LeftButton:
                this->B = VK_LBUTTON;
                get_scan_code(VK_LBUTTON, buffer, 256);
                break;
            case Qt::MouseButton::RightButton:
                this->B = VK_RBUTTON;
                get_scan_code(VK_RBUTTON, buffer, 256);
                break;
            case Qt::MouseButton::MiddleButton:
                this->B = VK_MBUTTON;
                get_scan_code(VK_MBUTTON, buffer, 256);
                break;
            default:
                qDebug() << "Some Error Occured No Legal Mouse Button found" << Qt::endl;
                valid = false;
            }
            if(valid)
            ui->bmap->setText(QString(buffer));
        }
    }
    else if(sender == ui->Ltmap) {
        if(event->type() == QEvent::KeyRelease) {
            QKeyEvent* key_press = static_cast<QKeyEvent*>(event);
            this->LT = key_press->nativeVirtualKey();
            char buffer[256];
            get_scan_code(key_press->nativeVirtualKey(), buffer, 256);
            ui->Ltmap->setText(QString(buffer));
        }
        else if(event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent* mouse_press = static_cast<QMouseEvent*>(event);
            char buffer[256];
            bool valid = true;
            UINT button = mouse_press->button();
            switch(button) {
            case Qt::MouseButton::LeftButton:
                this->LT = VK_LBUTTON;
                get_scan_code(VK_LBUTTON, buffer, 256);
                break;
            case Qt::MouseButton::RightButton:
                this->LT = VK_RBUTTON;
                get_scan_code(VK_RBUTTON, buffer, 256);
                break;
            case Qt::MouseButton::MiddleButton:
                this->LT = VK_MBUTTON;
                get_scan_code(VK_MBUTTON, buffer, 256);
                break;
            default:
                qDebug() << "Some Error Occured No Legal Mouse Button found" << Qt::endl;
                valid = false;
            }
            if(valid)
            ui->Ltmap->setText(QString(buffer));
        }
    }
    else if(sender == ui->Rtmap) {
        if(event->type() == QEvent::KeyRelease) {
            QKeyEvent* key_press = static_cast<QKeyEvent*>(event);
            this->RT = key_press->nativeVirtualKey();
            char buffer[256];
            get_scan_code(key_press->nativeVirtualKey(), buffer, 256);
            ui->Rtmap->setText(QString(buffer));
        }
        else if(event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent* mouse_press = static_cast<QMouseEvent*>(event);
            char buffer[256];
            bool valid = true;
            UINT button = mouse_press->button();
            switch(button) {
            case Qt::MouseButton::LeftButton:
                this->RT = VK_LBUTTON;
                get_scan_code(VK_LBUTTON, buffer, 256);
                break;
            case Qt::MouseButton::RightButton:
                this->RT = VK_RBUTTON;
                get_scan_code(VK_RBUTTON, buffer, 256);
                break;
            case Qt::MouseButton::MiddleButton:
                this->RT = VK_MBUTTON;
                get_scan_code(VK_MBUTTON, buffer, 256);
                break;
            default:
                qDebug() << "Some Error Occured No Legal Mouse Button found" << Qt::endl;
                valid = false;
            }
            if(valid)
            ui->Rtmap->setText(QString(buffer));
        }
    }
    else if(sender == ui->dupmap) {
        if(event->type() == QEvent::KeyRelease) {
            QKeyEvent* key_press = static_cast<QKeyEvent*>(event);
            this->DUP = key_press->nativeVirtualKey();
            char buffer[256];
            get_scan_code(key_press->nativeVirtualKey(), buffer, 256);
            ui->dupmap->setText(QString(buffer));
        }
        else if(event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent* mouse_press = static_cast<QMouseEvent*>(event);
            char buffer[256];
            bool valid = true;
            UINT button = mouse_press->button();
            switch(button) {
            case Qt::MouseButton::LeftButton:
                this->DUP = VK_LBUTTON;
                get_scan_code(VK_LBUTTON, buffer, 256);
                break;
            case Qt::MouseButton::RightButton:
                this->DUP = VK_RBUTTON;
                get_scan_code(VK_RBUTTON, buffer, 256);
                break;
            case Qt::MouseButton::MiddleButton:
                this->DUP = VK_MBUTTON;
                get_scan_code(VK_MBUTTON, buffer, 256);
                break;
            default:
                qDebug() << "Some Error Occured No Legal Mouse Button found" << Qt::endl;
                valid = false;
            }
            if(valid)
            ui->dupmap->setText(QString(buffer));
        }
    }
    else if(sender == ui->ddownmap) {
        if(event->type() == QEvent::KeyRelease) {
            QKeyEvent* key_press = static_cast<QKeyEvent*>(event);
            this->DDOWN = key_press->nativeVirtualKey();
            char buffer[256];
            get_scan_code(key_press->nativeVirtualKey(), buffer, 256);
            ui->ddownmap->setText(QString(buffer));
        }
        else if(event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent* mouse_press = static_cast<QMouseEvent*>(event);
            char buffer[256];
            bool valid = true;
            UINT button = mouse_press->button();
            switch(button) {
            case Qt::MouseButton::LeftButton:
                this->DDOWN = VK_LBUTTON;
                get_scan_code(VK_LBUTTON, buffer, 256);
                break;
            case Qt::MouseButton::RightButton:
                this->DDOWN = VK_RBUTTON;
                get_scan_code(VK_RBUTTON, buffer, 256);
                break;
            case Qt::MouseButton::MiddleButton:
                this->DDOWN = VK_MBUTTON;
                get_scan_code(VK_MBUTTON, buffer, 256);
                break;
            default:
                qDebug() << "Some Error Occured No Legal Mouse Button found" << Qt::endl;
                valid = false;
            }
            if(valid)
            ui->ddownmap->setText(QString(buffer));
        }
    }
    else if(sender == ui->drightmap) {
        if(event->type() == QEvent::KeyRelease) {
            QKeyEvent* key_press = static_cast<QKeyEvent*>(event);
            this->DRIGHT = key_press->nativeVirtualKey();
            char buffer[256];
            get_scan_code(key_press->nativeVirtualKey(), buffer, 256);
            ui->drightmap->setText(QString(buffer));
        }
        else if(event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent* mouse_press = static_cast<QMouseEvent*>(event);
            char buffer[256];
            bool valid = true;
            UINT button = mouse_press->button();
            switch(button) {
            case Qt::MouseButton::LeftButton:
                this->DRIGHT = VK_LBUTTON;
                get_scan_code(VK_LBUTTON, buffer, 256);
                break;
            case Qt::MouseButton::RightButton:
                this->DRIGHT = VK_RBUTTON;
                get_scan_code(VK_RBUTTON, buffer, 256);
                break;
            case Qt::MouseButton::MiddleButton:
                this->DRIGHT = VK_MBUTTON;
                get_scan_code(VK_MBUTTON, buffer, 256);
                break;
            default:
                qDebug() << "Some Error Occured No Legal Mouse Button found" << Qt::endl;
                valid = false;
            }
            if(valid)
            ui->drightmap->setText(QString(buffer));
        }
    }
    else if(sender == ui->dleftmap) {
        if(event->type() == QEvent::KeyRelease) {
            QKeyEvent* key_press = static_cast<QKeyEvent*>(event);
            this->DLEFT = key_press->nativeVirtualKey();
            char buffer[256];
            get_scan_code(key_press->nativeVirtualKey(), buffer, 256);
            ui->dleftmap->setText(QString(buffer));
        }
        else if(event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent* mouse_press = static_cast<QMouseEvent*>(event);
            char buffer[256];
            bool valid = true;
            UINT button = mouse_press->button();
            switch(button) {
            case Qt::MouseButton::LeftButton:
                this->DLEFT = VK_LBUTTON;
                get_scan_code(VK_LBUTTON, buffer, 256);
                break;
            case Qt::MouseButton::RightButton:
                this->DLEFT = VK_RBUTTON;
                get_scan_code(VK_RBUTTON, buffer, 256);
                break;
            case Qt::MouseButton::MiddleButton:
                this->DLEFT = VK_MBUTTON;
                get_scan_code(VK_MBUTTON, buffer, 256);
                break;
            default:
                qDebug() << "Some Error Occured No Legal Mouse Button found" << Qt::endl;
                valid = false;
            }
            if(valid)
            ui->dleftmap->setText(QString(buffer));
        }
    }
    return QWidget::eventFilter(sender,event);
}

void Preferences::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return || e->key() == Qt::Key_Escape)
        return;
    return QDialog::keyPressEvent(e);
}

void Preferences::install_event_filter() {
    ui->xmap->installEventFilter(this);
    ui->ymap->installEventFilter(this);
    ui->amap->installEventFilter(this);
    ui->bmap->installEventFilter(this);
    ui->Ltmap->installEventFilter(this);
    ui->Rtmap->installEventFilter(this);
    ui->ddownmap->installEventFilter(this);
    ui->dupmap->installEventFilter(this);
    ui->drightmap->installEventFilter(this);
    ui->dleftmap->installEventFilter(this);
}

Preferences::~Preferences()
{
    delete ui;
}
