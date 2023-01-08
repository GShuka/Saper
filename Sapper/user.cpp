#include "user.h"

User::User()
{
	id = -1;
	loseCount = 0;
}

void User::addGame(GameInHistory game)
{
	// Если добавляется поражение - увеличить счётчик поражений
	if (!game.victory)
		loseCount++;
	// Если победа - вставить в отсортированный по длительности игры массив
	else
	{
		int i; 
		for (i = 0; i < victories.size(); i++)
			if (victories[i].time > game.time)
				break;
		victories.insert(i, game);
	}
}

QDataStream &operator << (QDataStream &out, const User &cur)
{
	out << cur.id
		<< cur.username
		<< cur.password
		<< cur.loseCount
		<< cur.victories.size();
	for (int i = 0; i < cur.victories.size(); i++)
	{
		out << cur.victories[i].number
			<< cur.victories[i].minesCount
			<< cur.victories[i].time;
	}

	return out;
}

QDataStream &operator >> (QDataStream &in, User &cur)
{
	int count;
	in  >> cur.id
		>> cur.username
		>> cur.password
		>> cur.loseCount
		>> count;
	cur.victories.resize(count);
	for (int i = 0; i < cur.victories.size(); i++)
	{
		in  >> cur.victories[i].number
			>> cur.victories[i].minesCount
			>> cur.victories[i].time;
		cur.victories[i].victory = true;
	}
	return in;
}