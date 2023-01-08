#pragma once
#include "user.h"
#include <QFile>
#include <QIODevice>

class Database
{
private:
	int nextUserId;					// ������������� ��� ������ ������������
	int nextGameNumber;				// ����� ��������� ����
	QVector <User> users;			// ������

public:
	Database();

	// �������� ����
	void addGame(GameInHistory game, int userId);

	// �������� ������������
	int addUser(QString username, QString password);

	// ������������ ������������
	int login(QString username, QString password);

	// �������� ��� ������������ �� ��������������
	QString username(int userId);

	// �������� ���������� ��������� ������������
	int loseCount(int userId);

	// �������� ���������� � ������� ������������
	QVector <GameInHistory> victories(int userId);

	// ��������� ���� ������
	void load();

	// ��������� ���� ������
	void save();
};
