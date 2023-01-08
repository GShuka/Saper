#ifndef SAPPER_H
#define SAPPER_H

#include <QtGui/QMainWindow>
#include "ui_sapper.h"
#include <QFile>
#include <QTimer>
#include <QMouseEvent>
#include <QTextCodec>
#include <time.h>
#include "cell.h"
#include "database.h"
#include "registration.h"
#include "authorization.h"

class Sapper : public QMainWindow
{
	Q_OBJECT

public:
	Sapper(QWidget *parent = 0, Qt::WFlags flags = 0);
	~Sapper();
	
	// ���������������� ������������
	bool registerUser(QString username, QString password);

	// ���������� ������������
	bool loginUser(QString username, QString password);

private:
	Ui::SapperClass ui;

	class Registration *registrationForm;	// ����� �����������
	class Authorization *authorizationForm;	// ����� �����������

	int userId;								// ������������� �������� ������ (-1 ��� �����)
	Database db;							// ���� ������

	static const int width = 10;			// ������ ����
	static const int height = 10;			// ������ ����
	static const int minMineCount = 10;		// ����������� ���������� ���
	static const int maxAddMineCount = 5;	// ������������ �������������� ���������� ���

	QString defaultInfo;					// ����� �� ��������� ��� ����� ����������

	int curMineCount;						// ���������� ��� � ������� ����
	bool minesField[height][width];			// ���� ��� (true, ���� � ������ i, j ����)
	Cell fieldStatus[height][width];		// ��������� ����� ���� ��� ������
	bool firstClick;						// ������ ���� � ����
	QTimer timer;							// ������
	bool gameInProgress;					// ���� ����
	int closedCellCount;					// ���������� �������� �����

	// ��������� ����� ��������� �� ������� ����
	bool inBorders(int x, int y);

	// ���������� ����
	void setMines();

	// ������� ����� ������� ���� �� ������ x, y
	void cellLeftClick(int x, int y);

	// ������� ������ ������� ���� �� ������ x, y
	void cellRightClick(int x, int y);

	// ������� ������
	void open(int x, int y);

	// ��������� ���������� ��� ����� ������
	int mineCount(int x, int y);

	// ��������� ����
	void finishGame(bool victory);

	// ���������� ������� �� ������ ����
	bool eventFilter(QObject *obj, QEvent *event);

	// ��������� �������� ����������
	void closeEvent(QCloseEvent *event);
	
	// ��������� ��������� �������� ����
	void resizeEvent(QResizeEvent *event);

private slots:
	// ������ ����� ����
	void startGame();
	// ������ ������� �������
	void secondGone();
	// �������� ����� �����������
	void showRegistrationForm();
	// �����������/����� �� ��.������
	void authorization();
	// �������� ������� ������
	void showProfile();
};

#endif // SAPPER_H
