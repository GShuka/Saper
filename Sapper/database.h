#pragma once
#include "user.h"
#include <QFile>
#include <QIODevice>

class Database
{
private:
	int nextUserId;					// Идентификатор для нового пользователя
	int nextGameNumber;				// Номер следующей игры
	QVector <User> users;			// Игроки

public:
	Database();

	// Добавить игру
	void addGame(GameInHistory game, int userId);

	// Добавить пользователя
	int addUser(QString username, QString password);

	// Авторизовать пользователя
	int login(QString username, QString password);

	// Получить имя пользователя по идентификатору
	QString username(int userId);

	// Получить количество поражений пользователя
	int loseCount(int userId);

	// Получить информацию о победах пользователя
	QVector <GameInHistory> victories(int userId);

	// Загрузить базу данных
	void load();

	// Выгрузить базу данных
	void save();
};
