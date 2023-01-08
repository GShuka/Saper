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
	
	// Зарегистрировать пользователя
	bool registerUser(QString username, QString password);

	// Залогинить пользователя
	bool loginUser(QString username, QString password);

private:
	Ui::SapperClass ui;

	class Registration *registrationForm;	// Форма регистрации
	class Authorization *authorizationForm;	// Форма авторизации

	int userId;								// Идентификатор текущего игрока (-1 для гостя)
	Database db;							// База данных

	static const int width = 10;			// Ширина поля
	static const int height = 10;			// Высота поля
	static const int minMineCount = 10;		// Минимальное количество мин
	static const int maxAddMineCount = 5;	// Максимальное дополнительное количество мин

	QString defaultInfo;					// Текст по умолчанию для блока информации

	int curMineCount;						// Количество мин в текущей игре
	bool minesField[height][width];			// Поле мин (true, если в ячейке i, j мина)
	Cell fieldStatus[height][width];		// Состояние ячеек поля для игрока
	bool firstClick;						// Первый клик в игре
	QTimer timer;							// Таймер
	bool gameInProgress;					// Игра идет
	int closedCellCount;					// Количество закрытых ячеек

	// Проверить выход координат за границы поля
	bool inBorders(int x, int y);

	// Расставить мины
	void setMines();

	// Нажатие левой кнопкой мыши на ячейку x, y
	void cellLeftClick(int x, int y);

	// Нажатие правой кнопкой мыши на ячейку x, y
	void cellRightClick(int x, int y);

	// Открыть ячейку
	void open(int x, int y);

	// Посчитать количество мин возле ячейки
	int mineCount(int x, int y);

	// Завершить игру
	void finishGame(bool victory);

	// Обработчик нажатия на ячейки поля
	bool eventFilter(QObject *obj, QEvent *event);

	// Обработка закрытия приложения
	void closeEvent(QCloseEvent *event);
	
	// Обработка изменения размеров окна
	void resizeEvent(QResizeEvent *event);

private slots:
	// Начать новую игру
	void startGame();
	// Прошла секунда таймера
	void secondGone();
	// Показать форму регистрации
	void showRegistrationForm();
	// Авторизация/выход из уч.записи
	void authorization();
	// Показать профиль игрока
	void showProfile();
};

#endif // SAPPER_H
