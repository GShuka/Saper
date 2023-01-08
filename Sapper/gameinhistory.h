#include <QString>

struct GameInHistory
{
	int number;			// Номер игры
	bool victory;		// Флаг победы (true, если выигрыш)
	int minesCount;		// Количество мин
	int time;			// Затраченное время (в секундах)
};