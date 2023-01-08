#include "database.h"

Database::Database()
{
	nextUserId = 1;
	nextGameNumber = 1;
}	

void Database::addGame(GameInHistory game, int userId)
{
	// Если игра сыграна не гостем
	if (userId != -1)
	{
		// Задать ей номер
		game.number = nextGameNumber;

		// Найти пользователя с заданным идентификатором и добавить ему игру
		for (int i = 0; i < users.size(); i++)
			if (users[i].id == userId)
				users[i].addGame(game);
	}
	nextGameNumber++;
}

int Database::addUser(QString username, QString password)
{
	// Проверить уникальность имени
	for (int i = 0; i < users.size(); i++)
		if (users[i].username == username)
			return -1;

	User newUser;
	// Присвоить идентификатор новому пользователю
	newUser.id = nextUserId;
	nextUserId++;
	// Записать введенные данные
	newUser.username = username;
	newUser.password = password;
	// Добавить в базу данных
	users.append(newUser);

	// Вернуть идентификатор
	return newUser.id;
}

int Database::login(QString username, QString password)
{
	// Проверить наличие пользователя с заданными данными
	for (int i = 0; i < users.size(); i++)
		if (users[i].username == username && users[i].password == password)
			return users[i].id;
	// Если пользователь не найден - возвращаем -1
	return -1;
}

QString Database::username(int userId)
{	
	// Найти пользователя с заданным идентификатором
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
	// Если пользователь не найден - возвращаем -1
	return -1;
}

QVector <GameInHistory> Database::victories(int userId)
{
	for (int i = 0; i < users.size(); i++)
		if (users[i].id == userId)
			return users[i].victories;
	// Если пользователь не найден - возвращаем -1
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