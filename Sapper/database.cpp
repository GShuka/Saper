#include "database.h"

Database::Database()
{
	nextUserId = 1;
	nextGameNumber = 1;
}	

void Database::addGame(GameInHistory game, int userId)
{
	// ���� ���� ������� �� ������
	if (userId != -1)
	{
		// ������ �� �����
		game.number = nextGameNumber;

		// ����� ������������ � �������� ��������������� � �������� ��� ����
		for (int i = 0; i < users.size(); i++)
			if (users[i].id == userId)
				users[i].addGame(game);
	}
	nextGameNumber++;
}

int Database::addUser(QString username, QString password)
{
	// ��������� ������������ �����
	for (int i = 0; i < users.size(); i++)
		if (users[i].username == username)
			return -1;

	User newUser;
	// ��������� ������������� ������ ������������
	newUser.id = nextUserId;
	nextUserId++;
	// �������� ��������� ������
	newUser.username = username;
	newUser.password = password;
	// �������� � ���� ������
	users.append(newUser);

	// ������� �������������
	return newUser.id;
}

int Database::login(QString username, QString password)
{
	// ��������� ������� ������������ � ��������� �������
	for (int i = 0; i < users.size(); i++)
		if (users[i].username == username && users[i].password == password)
			return users[i].id;
	// ���� ������������ �� ������ - ���������� -1
	return -1;
}

QString Database::username(int userId)
{	
	// ����� ������������ � �������� ���������������
	for (int i = 0; i < users.size(); i++)
		if (users[i].id == userId)
			return users[i].username;
	return "";
}

int Database::loseCount(int userId)
{
	for (int i = 0; i < users.size(); i++)
		if (users[i].id == userId)
			return users[i].loseCount;
	// ���� ������������ �� ������ - ���������� -1
	return -1;
}

QVector <GameInHistory> Database::victories(int userId)
{
	for (int i = 0; i < users.size(); i++)
		if (users[i].id == userId)
			return users[i].victories;
	// ���� ������������ �� ������ - ���������� -1
	return QVector <GameInHistory>();
}

void Database::load()
{
	QFile file("db.spr");
	if (file.open(QIODevice::ReadOnly))
	{
		int count;

		QDataStream in(&file);
		in  >> nextUserId
			>> nextGameNumber
			>> count;

		users.resize(count);
		for (int i = 0; i < users.size(); i++)
			in >> users[i];
	}
}

void Database::save()
{
	QFile file("db.spr");
	if (file.open(QIODevice::WriteOnly))
	{
		QDataStream out(&file);
		out << nextUserId
			<< nextGameNumber
			<< users.size();

		for (int i = 0; i < users.size(); i++)
			out << users[i];
	}
}