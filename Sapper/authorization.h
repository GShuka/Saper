#ifndef AUTHORIZATION_H
#define AUTHORIZATION_H

#include <QWidget>
#include "ui_authorization.h"
#include "Sapper.h"

class Authorization : public QWidget
{
	Q_OBJECT

public:
	Authorization(QWidget *parent = 0);
	~Authorization();

	// �������� ���� �����
	void clear();

private:
	Ui::Authorization ui;
	class Sapper *mainWindow;

private slots:
	// ������� �����������
	void authorizationConfirm();
};

#endif // AUTHORIZATION_H
