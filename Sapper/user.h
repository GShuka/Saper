#include "GameInHistory.h"
#include <QVector>
#include <QDataStream>

class User
{
public:
	User();

	int id;								// Идентификатор
	QString username;					// Имя
	QString password;					// Пароль
	int loseCount;						// Количество поражений
	QVector <GameInHistory> victories;	// Победы (отсортированы по длительности)

	// Добавить сыгранную игру к пользователю
	void addGame(GameInHistory game);
};

// Вывод в файл
QDataStream &operator << (QDataStream &out, const User &cur);

// Ввод из файла
QDataStream &operator >> (QDataStream &in, User &cur);