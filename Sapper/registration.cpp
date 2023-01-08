#include "registration.h"

Registration::Registration(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	mainWindow = (Sapper *) parent;

	// Сокрытие вводимых символов пароля
	ui.password->setEchoMode(QLineEdit::Password);
	ui.confirmPassword->setEchoMode(QLineEdit::Password);

	connect(ui.registration, SIGNAL(clicked()), this, SLOT(registrationConfirm()));
}

Registration::~Registration()
{

}

void Registration::clear()
{
	ui.username->clear();
	ui.password->clear();
	ui.confirmPassword->clear();
}

void Registration::registrationConfirm()
{
	// Проверка логина на пустоту
	if (ui.username->text().isEmpty())
	{
		QMessageBox::critical(this, "Ошибка", "Введите имя пользователя");
		return;
	}
	// Проверка количества символов пароля
	if (ui.password->text().length() < 8)
	{
		QMessageBox::critical(this, "Ошибка", "Длина пароля должна быть не менее 8 символов");
		return;
	}
	// Проверка соответствия паролей
	if (ui.password->text() != ui.confirmPassword->text())
	{
		QMessageBox::critical(this, "Ошибка", "Пароли не соответствуют");
		return;
	}
	// Проверка доступности имени пользователя
	if (!mainWindow->registerUser(ui.username->text(), ui.password->text()))
	{
		QMessageBox::critical(this, "Ошибка", "Имя пользователя уже используется");
		return;
	}
	this->close();
}