#include "sapper.h"

Sapper::Sapper(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	// Установка кодеков для русскоязычного текста
	QTextCodec *tc=QTextCodec::codecForName("CP1251");
	QTextCodec::setCodecForCStrings(tc);

	// Обновить генератор случайных чисел
	qsrand(time(NULL));

	// Установить стили пользовательского интерфейса
	QFile styleFile("style.css");
	styleFile.open(QFile::ReadOnly);
	QString qss = styleFile.readAll();
	qApp->setStyleSheet(qss);
    
	// Фон окна
	QPixmap background("img/background.gif");
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);
    this->setAutoFillBackground(true);

	// Запомнить текст по умолчанию для блока информации
	defaultInfo = ui.info->text();

	// Установить слушателя нажатия на ячейку
	for (int i = 0; i < Sapper::height; i++)
	{
		for (int j = 0; j < Sapper::width; j++)
		{
			QPushButton * cell = this->findChild<QPushButton *>("cell" + QString::number(i) + QString::number(j));
			cell->installEventFilter(this);
		}
	}

	// Установить связи между сигналами и обработчиками (слотами)
	connect(&timer,				SIGNAL(timeout()), this, SLOT(secondGone()));
	connect(ui.newGame,			SIGNAL(clicked()), this, SLOT(startGame()));
	connect(ui.registration,	SIGNAL(clicked()), this, SLOT(showRegistrationForm()));
	connect(ui.login,			SIGNAL(clicked()), this, SLOT(authorization()));
	connect(ui.profile,			SIGNAL(clicked()), this, SLOT(showProfile()));

	// Инициализировать модальные окна
	registrationForm = new Registration(this);
    registrationForm->setWindowFlags(Qt::Dialog);
    registrationForm->setWindowModality(Qt::WindowModal);
	authorizationForm = new Authorization(this);
	authorizationForm->setWindowFlags(Qt::Dialog);
	authorizationForm->setWindowModality(Qt::WindowModal);
	
	// Запретить изменять размеры колонок таблицы
	ui.table->horizontalHeader()->setResizeMode(QHeaderView::Fixed);

	// Текущий игрок - гость
	userId = -1;
	// Для гостя не видно кнопки "Профиль"
	ui.profile->setVisible(false);

	// Загрузить базу данных
	db.load();

	// Начать новую игру
	startGame();
}

Sapper::~Sapper()
{

}

bool Sapper::inBorders(int x, int y)
{
	return x >= 0 && y >= 0 && x < Sapper::height && y < Sapper::width;
}

void Sapper::setMines()
{
	int x;
	int y;

	// Очистить старое минное поле
	for (int i = 0; i < Sapper::height; i++)
		for (int j = 0; j < Sapper::width; j++)
			minesField[i][j] = false;

	// Сгенерировать новое число мин
	curMineCount = minMineCount + qrand() % maxAddMineCount;

	// Получить случайные позиции мин
	for (int i = 0; i < curMineCount; i++)
	{
		// Генерировать случайные координаты до тех пор, пока они не будут указывать на свободную ячейку
		do 
		{
			x = qrand() % Sapper::height;
			y = qrand() % Sapper::width;
		} while (minesField[x][y] == true);

		// Установить мину
		minesField[x][y] = true;
	}

	// Вывести в текстовое поле количество мин
	QString text = ui.info->text();
	text.replace(text.indexOf("<br/><br/>") + QString("<br/><br/>Количество мин - ").length(), 2, QString::number(curMineCount));
	ui.info->setText(text);
}

void Sapper::startGame()
{
	// Вернуть блок информации в начальное состояние
	ui.info->setText(defaultInfo);

	// Установить мины
	setMines();

	// Обновить статус ячеек поля
	for (int i = 0; i < Sapper::height; i++)
	{
		for (int j = 0; j < Sapper::width; j++)
		{
			fieldStatus[i][j] = CLOSED;
			QPushButton * cell = this->findChild<QPushButton *>("cell" + QString::number(i) + QString::number(j));
			cell->setStyleSheet("");
			cell->setText("");
		}
	}
	closedCellCount = Sapper::height * Sapper::width;

	// Первого клика ещё не было
	firstClick = false;

	// Игра началась
	gameInProgress = true;

	// Остановить таймер (если начали новую игру, не закончив старую)
	timer.stop();

	// Переключиться к вкладке с игрой
	ui.stackedWidget->setCurrentIndex(0);
}	

void Sapper::cellLeftClick(int x, int y)
{	
	// Ячейка
	QPushButton * cell = this->findChild<QPushButton *>("cell" + QString::number(x) + QString::number(y));

	// Если ячейка была помечена - снять флажок
	if (fieldStatus[x][y] == MARKED)
	{
		fieldStatus[x][y] = CLOSED;

		// Удалить флажок
		cell->setStyleSheet("");
	}
	// Если ячейка была закрыта - открыть
	else if (fieldStatus[x][y] == CLOSED)
	{
		open(x, y);
	}
}

void Sapper::cellRightClick(int x, int y)
{
	// Ячейка
	QPushButton * cell = this->findChild<QPushButton *>("cell" + QString::number(x) + QString::number(y));
	
	// Если ячейка была закрыта - установить флажок
	if (fieldStatus[x][y] == CLOSED)
	{
		fieldStatus[x][y] = MARKED;

		// Добавить флажок
		cell->setStyleSheet("background-image: url(img/red.png);");
	}
	// Если ячейка была помечена - снять флажок
	else if (fieldStatus[x][y] == MARKED)
	{
		fieldStatus[x][y] = CLOSED;

		// Удалить флажок
		cell->setStyleSheet("");
	}
}

void Sapper::open(int x, int y)
{
	// Проверить на выход координат за границы поля
	if (!inBorders(x, y))
		return;

	// Если открыли мину - поражение
	if (minesField[x][y] == true)
		finishGame(false);
	else
	{
		fieldStatus[x][y] = OPENED;
		closedCellCount--;
		// Ячейка
		QPushButton * cell = this->findChild<QPushButton *>("cell" + QString::number(x) + QString::number(y));
		// Установить новый цвет фона
		cell->setStyleSheet("background-color: #fff;");

		// Установить количество мин
		int count = mineCount(x, y);
		if (count == 0)
		{
			// Если мин нет - открыть все соседние ячейки
			for (int i = -1; i <= 1; i++)
				for (int j = -1; j <= 1; j++)
					if (!(fieldStatus[x+i][y+j] == OPENED))
						open(x + i, y + j);
		}
		else
			cell->setText(QString::number(count));

		// Если количество ячеек равно числу мин - мы выиграли!!
		if (closedCellCount == curMineCount)
			finishGame(true);
	}
}

int Sapper::mineCount(int x, int y)
{
	int count = 0;

	// Посчитать количество мин в квадрате 3х3 с центром в переданной ячейке
	for (int i = -1; i <= 1; i++)
		for (int j = -1; j <= 1; j++)
			if (inBorders(x + i, y + j) && minesField[x + i][y + j] == true)
				count++;

	return count;
}

void Sapper::finishGame(bool victory)
{
	// Если игра уже была завершена - ничего не делать
	if (!gameInProgress)
		return;

	// Игра закончилась
	gameInProgress = false;
	timer.stop();

	// Показать все мины при поражении
	if (!victory)
	{
		for (int i = 0; i < Sapper::height; i++)
		{
			for (int j = 0; j < Sapper::width; j++)
			{
				if (minesField[i][j] == true)
				{
					// Ячейка
					QPushButton * cell = this->findChild<QPushButton *>("cell" + QString::number(i) + QString::number(j));
					// Установить белый цвет фона и изображение мины
					cell->setStyleSheet("background-color: #fff;background-image: url(img/mine.png);");
				}
			}
		}
	}

	// Сохранить игру в историю
	GameInHistory gameInfo;
	gameInfo.minesCount = curMineCount;
	gameInfo.victory = victory;
	// Получить затраченное время
	QString time = ui.info->text().right(8 + QString("</span></p></body></html>").length()).left(8);
	int s = time[6].digitValue() * 10 + time[7].digitValue();
	int m = time[3].digitValue() * 10 + time[4].digitValue();
	int h = time[0].digitValue() * 10 + time[1].digitValue();
	gameInfo.time = s + m * 60 + h * 3600;
	db.addGame(gameInfo, userId);

	// Вывести результат игры
	ui.info->setText(ui.info->text() + "Результат - " + (victory ? "Победа" : "Поражение"));
}

bool Sapper::eventFilter(QObject *obj, QEvent *event)
{
	// Если нажали кнопкой мыши по ячейке поля
    if (event->type() == QEvent::MouseButtonPress && obj->objectName().startsWith("cell"))
	{
		// Если игра не идет (закончилась) - не реагировать на нажатия
		if (!gameInProgress)
			return true;

		// Вычислить координаты нажатой ячейки
		int x = obj->objectName()[4].digitValue();
		int y = obj->objectName()[5].digitValue();

		QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
		// Запустить таймер при первом клике по полю
		if ((mouseEvent->button() == Qt::LeftButton || mouseEvent->button() == Qt::RightButton) && !firstClick)
		{
			timer.setInterval(1000);
			timer.start();
			firstClick = true;
		}

		// Если нажата правая кнопка мыши
		if (mouseEvent->button() == Qt::RightButton)
			cellRightClick(x, y);
		// Если нажата левая кнопка мыши
		else if (mouseEvent->button() == Qt::LeftButton)
			cellLeftClick(x, y);

		return true;
	}
	// Иначе стандартная обработка
	else
	{
		return QObject::eventFilter(obj, event);
	}
}

void Sapper::closeEvent(QCloseEvent *event)
{
	// Сохранить базу данных
	db.save();

	// Освободить память
	delete registrationForm;
	delete authorizationForm;

	event->accept();
}

void Sapper::resizeEvent(QResizeEvent *event)
{
	int windowWidth = event->size().width();
	// Перемещение кнопок входа и регистрации
	ui.login->move(windowWidth - 5 - ui.login->width(), ui.login->pos().y());
	ui.registration->move(windowWidth - 6 - ui.login->width() - ui.registration->width(), ui.registration->pos().y());
	// Перемещение имени пользователя
	ui.usernameLabel->move(windowWidth - 14 - ui.login->width() - ui.usernameLabel->width(), ui.usernameLabel->pos().y());
	// Изменение размеров таблицы
	ui.stackedWidget->resize(windowWidth, ui.stackedWidget->height());
	ui.whiteBg->resize(windowWidth - 80, ui.whiteBg->height());
	ui.table->resize(windowWidth - 120, ui.table->height());
	// Задать размеры колонок таблицы
	ui.table->setColumnWidth(0, ui.table->width() / 7);
	ui.table->setColumnWidth(1, ui.table->width() / 7 * 3);
	ui.table->setColumnWidth(2, ui.table->width() - (ui.table->width() / 7) * 4 - 2);
}

bool Sapper::registerUser(QString username, QString password)
{
	// Добавить пользователя в базу данных
	int id = db.addUser(username, password);
	// Если не удалось (имя пользователя занято) - завершить регистрацию
	if (id == -1)
		return false;
	// Войти с вновь введенными данными
	loginUser(username, password);
	return true;
}

bool Sapper::loginUser(QString username, QString password)
{
	// Проверить авторизационные данные пользователя
	int id = db.login(username, password);
	// Если не удалось авторизоваться - завершить авторизацию
	if (id == -1)
		return false;
	
	userId = id;
		
	// Вывести имя пользователя
	ui.usernameLabel->setText("Вы вошли как " + db.username(userId));
	// Отобразить кнопку "Профиль"
	ui.profile->setVisible(true);
	// Скрыть кнопку регистрации
	ui.registration->setVisible(false);
	// Кнопка входа теперь выход
	ui.login->setText("Выход");

	return true;
}

void Sapper::secondGone()
{
	// Получить текущее прошедшее время
	QString time = ui.info->text().right(8 + QString("</span></p></body></html>").length()).left(8);
	int s = time[6].digitValue() * 10 + time[7].digitValue();
	int m = time[3].digitValue() * 10 + time[4].digitValue();
	int h = time[0].digitValue() * 10 + time[1].digitValue();

	// Прибавить одну секунду
	s++;
	if (s == 60)
	{
		m++;
		s = 0;
	}
	if (m == 60)
	{
		h++;
		m = 0;
	}
	
	// Установить обновленное время
	QString newTime = QString::number(h / 10) + QString::number(h % 10) + ":"
					+ QString::number(m / 10) + QString::number(m % 10) + ":"
					+ QString::number(s / 10) + QString::number(s % 10);
	QString newText = ui.info->text().replace(ui.info->text().length() - QString("</span></p></body></html>").length() - 8, 8, newTime);
	ui.info->setText(newText);

	// Перезапустить таймер
	timer.start();
}

void Sapper::showRegistrationForm()
{
	registrationForm->clear();
	registrationForm->show();
}

void Sapper::authorization()
{
	// Если нужно авторизоваться
	if (userId == -1)
	{
		// Показать окно авторизации
		authorizationForm->clear();
		authorizationForm->show();
	}
	// Если нужно выйти
	else
	{
		userId = -1;
		
		// Очистить поле с именем пользователя
		ui.usernameLabel->setText("");
		// Скрыть кнопку "Профиль"
		ui.profile->setVisible(false);
		// Отобразить кнопку регистрации
		ui.registration->setVisible(true);
		// Кнопка выхода теперь вход
		ui.login->setText("Вход");
		// Переключиться к вкладке с игрой
		ui.stackedWidget->setCurrentIndex(0);
	}
}

void Sapper::showProfile()
{
	// Число поражений
	ui.loseCount->setText("Количество поражений - " + QString::number(db.loseCount(userId)));

	// Победы
	QVector <GameInHistory> victories = db.victories(userId);
	ui.winCount->setText("Успешных разминирований - " + QString::number(victories.size()));
	ui.table->setRowCount(victories.size());
	for (int i = 0; i < victories.size(); i++)
	{
		ui.table->setItem(i, 0, new QTableWidgetItem(QString::number(victories[i].number)));
		ui.table->setItem(i, 1, new QTableWidgetItem(QString::number(victories[i].minesCount)));
		ui.table->setItem(i, 2, new QTableWidgetItem(QString::number(victories[i].time)));
	}

	// Переключиться к вкладке профиля
	ui.stackedWidget->setCurrentIndex(1);
}