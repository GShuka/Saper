#include "authorization.h"

Authorization::Authorization(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	mainWindow = (Sapper *) parent;

	// Сокрытие вводимых символов пароля
	ui.password->setEchoMode(QLineEdit::Password);

	connect(ui.login, SIGNAL(clicked()), this, SLOT(authorizationConfirm()));
}

Authorization::~Authorization()
{

}

void Authorization::clear()
{
	ui.username->clear();
	ui.password->clear();
}

void Authorization::authorizationConfirm()
{
	// Проверка логина на пустоту
	if (ui.username->text().isEmpty())
	{
		QMessageBox::critical(this, "Ошибка", "Введите имя пользователя");
		return;
	}
	// Попытаться войти
	if (!mainWindow->loginUser(ui.username->text(), ui.password->text()))
	{
		QMessageBox::critical(this, "Ошибка", "Логин или пароль неверны");
		return;
	}
	this->close();
}