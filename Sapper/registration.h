#ifndef REGISTRATION_H
#define REGISTRATION_H

#include <QWidget>
#include <QLineEdit>
#include <QMessageBox>
#include "ui_registration.h"
#include "Sapper.h"

class Registration : public QWidget
{
	Q_OBJECT

public:
	Registration(QWidget *parent = 0);
	~Registration();

	// �������� ���� �����
	void clear();

private:
	Ui::Registration ui;
	class Sapper *mainWindow;

private slots:
	// ������� �����������
	void registrationConfirm();
};

#endif // REGISTRATION_H
