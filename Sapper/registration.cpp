#include "registration.h"

Registration::Registration(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	mainWindow = (Sapper *) parent;

	// �������� �������� �������� ������
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
	// �������� ������ �� �������
	if (ui.username->text().isEmpty())
	{
		QMessageBox::critical(this, "������", "������� ��� ������������");
		return;
	}
	// �������� ���������� �������� ������
	if (ui.password->text().length() < 8)
	{
		QMessageBox::critical(this, "������", "����� ������ ������ ���� �� ����� 8 ��������");
		return;
	}
	// �������� ������������ �������
	if (ui.password->text() != ui.confirmPassword->text())
	{
		QMessageBox::critical(this, "������", "������ �� �������������");
		return;
	}
	// �������� ����������� ����� ������������
	if (!mainWindow->registerUser(ui.username->text(), ui.password->text()))
	{
		QMessageBox::critical(this, "������", "��� ������������ ��� ������������");
		return;
	}
	this->close();
}