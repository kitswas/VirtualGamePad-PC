#include "preferences.hpp"
#include "../settings/input_types.hpp"
#include "../settings/settings.hpp"
#include "../settings/settings_key_variables.hpp"
#include "../settings/settings_singleton.hpp"
#include "ui_preferences.h"
#include "winuser.h"
#include <QKeyEvent>
#include <QLayout>
#include <QLineEdit>
#include <QProcess>
#include <QSlider>

Preferences::Preferences(QWidget *parent) : QWidget(parent), ui(new Ui::Preferences)
{
	ui->setupUi(this);
	install_event_filter();
	ui->horizontalSlider->adjustSize();
	ui->buttonBox->connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [this] {
		SettingsSingleton::instance().setMouseSensitivity(ui->horizontalSlider->value() * 100);
		qDebug() << SettingsSingleton::instance().mouseSensitivity();
		change_key_inputs();
	});
	ui->buttonBox->connect(ui->buttonBox, &QDialogButtonBox::rejected, this, [this] {
		load_keys();
		this->deleteLater();
	});
	ui->buttonBox->connect(ui->buttonBox, &QDialogButtonBox::accepted, this,
						   [this] { this->deleteLater(); });
	ui->buttonBox->setCenterButtons(true);
	ui->formLayout->setSizeConstraint(QLayout::SetMinimumSize);
	ui->formLayout->setHorizontalSpacing(50);
	ui->formLayout->setVerticalSpacing(10);
	ui->horizontalSlider->setValue(SettingsSingleton::instance().mouseSensitivity() / 100);
	Preferences::load_keys();
}

/**
 * Changes the mouse sensitivity or the cursor speed
 * @param value
 * The amount of mouse sensitivity you want to set.
 */
void Preferences::change_mouse_sensitivity(int value)
{
	SettingsSingleton::instance().setMouseSensitivity(value);
}

/**
 * This changes the keyboard maps and saves those changes in the config file.
 * This function is executed if the user presses ok button.
 */

void Preferences::change_key_inputs()
{
	auto &buttons = SettingsSingleton::instance().gamepadButtons();
	buttons[GamepadButtons::GamepadButtons_X].vk = this->temp[ui->xmap->objectName()];
	buttons[GamepadButtons::GamepadButtons_X].is_mouse_key =
		is_mouse_button(this->temp[ui->xmap->objectName()]);
	SettingsSingleton::instance().saveSetting(keymaps[setting_keys::keys::X],
											  this->temp[ui->xmap->objectName()]);
	/*------------------------------------------------------------*/
	buttons[GamepadButtons::GamepadButtons_Y].vk = this->temp[ui->ymap->objectName()];
	buttons[GamepadButtons::GamepadButtons_Y].is_mouse_key =
		is_mouse_button(this->temp[ui->ymap->objectName()]);
	SettingsSingleton::instance().saveSetting(keymaps[setting_keys::keys::Y],
											  this->temp[ui->ymap->objectName()]);
	/*------------------------------------------------------------*/
	buttons[GamepadButtons::GamepadButtons_A].vk = this->temp[ui->amap->objectName()];
	buttons[GamepadButtons::GamepadButtons_A].is_mouse_key =
		is_mouse_button(this->temp[ui->amap->objectName()]);
	SettingsSingleton::instance().saveSetting(keymaps[setting_keys::keys::A],
											  this->temp[ui->amap->objectName()]);
	/*------------------------------------------------------------*/
	buttons[GamepadButtons::GamepadButtons_B].vk = this->temp[ui->bmap->objectName()];
	buttons[GamepadButtons::GamepadButtons_B].is_mouse_key =
		is_mouse_button(this->temp[ui->bmap->objectName()]);
	SettingsSingleton::instance().saveSetting(keymaps[setting_keys::keys::B],
											  this->temp[ui->bmap->objectName()]);
	/*------------------------------------------------------------*/
	buttons[GamepadButtons::GamepadButtons_LeftShoulder].vk = this->temp[ui->Ltmap->objectName()];
	buttons[GamepadButtons::GamepadButtons_LeftShoulder].is_mouse_key =
		is_mouse_button(this->temp[ui->Ltmap->objectName()]);
	SettingsSingleton::instance().saveSetting(keymaps[setting_keys::keys::LSHDR],
											  this->temp[ui->Ltmap->objectName()]);
	/*------------------------------------------------------------*/
	buttons[GamepadButtons::GamepadButtons_RightShoulder].vk = this->temp[ui->Rtmap->objectName()];
	buttons[GamepadButtons::GamepadButtons_RightShoulder].is_mouse_key =
		is_mouse_button(this->temp[ui->Rtmap->objectName()]);
	SettingsSingleton::instance().saveSetting(keymaps[setting_keys::keys::RSHDR],
											  this->temp[ui->Rtmap->objectName()]);
	/*------------------------------------------------------------*/
	buttons[GamepadButtons::GamepadButtons_DPadDown].vk = this->temp[ui->ddownmap->objectName()];
	buttons[GamepadButtons::GamepadButtons_DPadDown].is_mouse_key =
		is_mouse_button(this->temp[ui->ddownmap->objectName()]);
	SettingsSingleton::instance().saveSetting(keymaps[setting_keys::keys::DPADDOWN],
											  this->temp[ui->ddownmap->objectName()]);
	/*------------------------------------------------------------*/
	buttons[GamepadButtons::GamepadButtons_DPadUp].vk = this->temp[ui->dupmap->objectName()];
	buttons[GamepadButtons::GamepadButtons_DPadUp].is_mouse_key =
		is_mouse_button(this->temp[ui->dupmap->objectName()]);
	SettingsSingleton::instance().saveSetting(keymaps[setting_keys::keys::DPADUP],
											  this->temp[ui->dupmap->objectName()]);
	/*------------------------------------------------------------*/
	buttons[GamepadButtons::GamepadButtons_DPadRight].vk = this->temp[ui->drightmap->objectName()];
	buttons[GamepadButtons::GamepadButtons_DPadRight].is_mouse_key =
		is_mouse_button(this->temp[ui->drightmap->objectName()]);
	SettingsSingleton::instance().saveSetting(keymaps[setting_keys::keys::DPADRIGHT],
											  this->temp[ui->drightmap->objectName()]);
	/*------------------------------------------------------------*/
	buttons[GamepadButtons::GamepadButtons_DPadLeft].vk = this->temp[ui->dleftmap->objectName()];
	buttons[GamepadButtons::GamepadButtons_DPadLeft].is_mouse_key =
		is_mouse_button(this->temp[ui->dleftmap->objectName()]);
	SettingsSingleton::instance().saveSetting(keymaps[setting_keys::keys::DPADLEFT],
											  this->temp[ui->dleftmap->objectName()]);
	/*-----------------------------------------------------------*/
	buttons[GamepadButtons::GamepadButtons_View].vk = this->temp[ui->viewmap->objectName()];
	buttons[GamepadButtons::GamepadButtons_View].is_mouse_key =
		is_mouse_button(this->temp[ui->viewmap->objectName()]);
	SettingsSingleton::instance().saveSetting(keymaps[setting_keys::keys::VIEW],
											  this->temp[ui->viewmap->objectName()]);
	/*-----------------------------------------------------------*/
	buttons[GamepadButtons::GamepadButtons_Menu].vk = this->temp[ui->menumap->objectName()];
	buttons[GamepadButtons::GamepadButtons_Menu].is_mouse_key =
		is_mouse_button(this->temp[ui->menumap->objectName()]);
	SettingsSingleton::instance().saveSetting(keymaps[setting_keys::keys::MENU],
											  this->temp[ui->menumap->objectName()]);
}

/**
 * Copies the name of the of corresponding key or virtual key code to the provided buffer.
 * @param vk
 * The virtual key code of the key you want to get.
 * @param a
 * The buffer to store the name of the key.
 * @param size
 * Size of the buffer(in char) in which the name is stored to ensure memory safety
 */
void Preferences::get_scan_code(WORD vk, char *a, int size)
{
	char sc = MapVirtualKeyA((UINT)vk, MAPVK_VK_TO_CHAR);
	if (sc >= '0' && sc <= 'Z')
	{
		strncpy_s(a, size, "", sizeof(""));
		strncpy_s(a, size, &sc, sizeof(sc));
	}
	else
	{
		strncpy_s(a, size, vk_maps[vk], sizeof(vk_maps[vk]));
	}
}

/**
 * Displays the key to which each button is mapped to.
 * Saves the initial key maps in variables that can be changed later if user wants to.
 */
void Preferences::load_keys()
{
	char buffer[256];
	this->temp[ui->xmap->objectName()] =
		SettingsSingleton::instance().gamepadButtons()[GamepadButtons::GamepadButtons_X].vk;
	get_scan_code(
		SettingsSingleton::instance().gamepadButtons()[GamepadButtons::GamepadButtons_X].vk, buffer,
		256);
	this->ui->xmap->setText(QString(buffer));
	/*------------------------------------------------------------*/
	this->temp[ui->ymap->objectName()] =
		SettingsSingleton::instance().gamepadButtons()[GamepadButtons::GamepadButtons_Y].vk;
	get_scan_code(SettingsSingleton::instance().gamepadButtons()[GamepadButtons_Y].vk, buffer, 256);
	this->ui->ymap->setText(QString(buffer));
	/*------------------------------------------------------------*/
	this->temp[ui->amap->objectName()] =
		SettingsSingleton::instance().gamepadButtons()[GamepadButtons::GamepadButtons_A].vk;
	get_scan_code(SettingsSingleton::instance().gamepadButtons()[GamepadButtons_A].vk, buffer, 256);
	this->ui->amap->setText(QString(buffer));
	/*------------------------------------------------------------*/
	this->temp[ui->bmap->objectName()] =
		SettingsSingleton::instance().gamepadButtons()[GamepadButtons::GamepadButtons_B].vk;
	get_scan_code(SettingsSingleton::instance().gamepadButtons()[GamepadButtons_B].vk, buffer, 256);
	this->ui->bmap->setText(QString(buffer));
	/*------------------------------------------------------------*/
	this->temp[ui->Rtmap->objectName()] =
		SettingsSingleton::instance()
			.gamepadButtons()[GamepadButtons::GamepadButtons_RightShoulder]
			.vk;
	get_scan_code(SettingsSingleton::instance().gamepadButtons()[GamepadButtons_RightShoulder].vk,
				  buffer, 256);
	this->ui->Rtmap->setText(QString(buffer));
	/*------------------------------------------------------------*/
	this->temp[ui->Ltmap->objectName()] =
		SettingsSingleton::instance()
			.gamepadButtons()[GamepadButtons::GamepadButtons_LeftShoulder]
			.vk;
	get_scan_code(SettingsSingleton::instance().gamepadButtons()[GamepadButtons_LeftShoulder].vk,
				  buffer, 256);
	this->ui->Ltmap->setText(QString(buffer));
	/*------------------------------------------------------------*/
	this->temp[ui->ddownmap->objectName()] =
		SettingsSingleton::instance().gamepadButtons()[GamepadButtons::GamepadButtons_DPadDown].vk;
	get_scan_code(SettingsSingleton::instance().gamepadButtons()[GamepadButtons_DPadDown].vk,
				  buffer, 256);
	this->ui->ddownmap->setText(QString(buffer));
	/*------------------------------------------------------------*/
	this->temp[ui->dupmap->objectName()] =
		SettingsSingleton::instance().gamepadButtons()[GamepadButtons::GamepadButtons_DPadUp].vk;
	get_scan_code(SettingsSingleton::instance().gamepadButtons()[GamepadButtons_DPadUp].vk, buffer,
				  256);
	this->ui->dupmap->setText(QString(buffer));
	/*------------------------------------------------------------*/
	this->temp[ui->drightmap->objectName()] =
		SettingsSingleton::instance().gamepadButtons()[GamepadButtons::GamepadButtons_DPadRight].vk;
	get_scan_code(SettingsSingleton::instance().gamepadButtons()[GamepadButtons_DPadRight].vk,
				  buffer, 256);
	this->ui->drightmap->setText(QString(buffer));
	/*------------------------------------------------------------*/
	this->temp[ui->dleftmap->objectName()] =
		SettingsSingleton::instance().gamepadButtons()[GamepadButtons::GamepadButtons_DPadLeft].vk;
	get_scan_code(SettingsSingleton::instance().gamepadButtons()[GamepadButtons_DPadLeft].vk,
				  buffer, 256);
	this->ui->dleftmap->setText(QString(buffer));
	/*-----------------------------------------------------------*/
	this->temp[ui->viewmap->objectName()] =
		SettingsSingleton::instance().gamepadButtons()[GamepadButtons::GamepadButtons_View].vk;
	get_scan_code(SettingsSingleton::instance().gamepadButtons()[GamepadButtons_View].vk, buffer,
				  256);
	this->ui->viewmap->setText(QString(buffer));
	/*-----------------------------------------------------------*/
	this->temp[ui->menumap->objectName()] =
		SettingsSingleton::instance().gamepadButtons()[GamepadButtons::GamepadButtons_Menu].vk;
	get_scan_code(SettingsSingleton::instance().gamepadButtons()[GamepadButtons_Menu].vk, buffer,
				  256);
	this->ui->menumap->setText(QString(buffer));
}

/**
 * The event filter virtual function is redefined to to filter for mouse and keyboard inputs when
 * user tries to change the button-key maps. Checks which object is sending the event and type of
 * event. If event is a keyboard or mouse button press then map and the object is button map then
 * get the virtual key code of the key pressed and store the change in a temporary variable.
 * @param sender
 * To get the address of the object that is triggering the event.
 * @param event
 * To capture the event that was triggered.
 * @return [bool] True if event is handled else False.
 */
bool Preferences::eventFilter(QObject *sender, QEvent *event)
{
	if (QLineEdit *ptr = qobject_cast<QLineEdit *>(sender); ptr)
	{
		if (event->type() == QEvent::KeyPress && ptr->text() == "")
		{
			const QKeyEvent *key_press = (QKeyEvent *)event;
			temp[ptr->objectName()] = key_press->nativeVirtualKey();
			char buffer[256];
			get_scan_code(key_press->nativeVirtualKey(), buffer, 256);
			ptr->setText(QString(buffer));
			return true;
		}
		else if (event->type() == QEvent::MouseButtonPress && ptr->text() == "")
		{
			const QMouseEvent *mouse_press = static_cast<QMouseEvent *>(event);
			char buffer[256];
			bool valid = false;
			UINT button = mouse_press->button();
			switch (button)
			{
			case Qt::MouseButton::LeftButton:
				temp[ptr->objectName()] = VK_LBUTTON;
				get_scan_code(VK_LBUTTON, buffer, 256);
				valid = true;
				break;
			case Qt::MouseButton::RightButton:
				temp[ptr->objectName()] = VK_RBUTTON;
				get_scan_code(VK_RBUTTON, buffer, 256);
				valid = true;
				break;
			case Qt::MouseButton::MiddleButton:
				temp[ptr->objectName()] = VK_MBUTTON;
				get_scan_code(VK_MBUTTON, buffer, 256);
				valid = true;
				break;
			default:
				qWarning() << "No legal mouse button found.";
			}
			if (valid)
				ptr->setText(QString(buffer));
			return true;
		}
		else
		{
			if ((event->type() == QEvent::KeyPress || event->type() == QEvent::MouseButtonPress) &&
				ptr->hasFocus())
				return true;
		}
	}
	return QWidget::eventFilter(sender, event);
}

/**
 * Make the QWidget box ignore the enter key and escape key presses when the focus is on button map
 * @param e
 * Capture the key_press event
 */
void Preferences::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return || e->key() == Qt::Key_Escape)
		return;
	return QWidget::keyPressEvent(e);
}

/**
 * Install the above event filter on all the button maps to capture the key presses when they have
 * the focus.
 */
void Preferences::install_event_filter()
{
	QList<QLineEdit *> lst = ui->KeyMaps->findChildren<QLineEdit *>();
	for (auto ptr = lst.begin(); ptr != lst.end(); ++ptr)
	{
		(*ptr)->installEventFilter(this);
	}
}

Preferences::~Preferences()
{
	delete ui;
}
