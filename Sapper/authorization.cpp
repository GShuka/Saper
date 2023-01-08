#include "authorization.h"

Authorization::Authorization(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	mainWindow = (Sapper *) parent;

	// �������� �������� �������� ������
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
	// �������� ������ �� �������
	if (ui.username->text().isEmpty())
	{
		QMessageBox::critical(this, "������", "������� ��� ������������");
		return;
	}
	// ���������� �����
	if (!mainWindow->loginUser(ui.username->text(), ui.password->text()))
	{
		QMessageBox::critical(this, "������", "����� ��� ������ �������");
		return;
	}
	this->close();
}