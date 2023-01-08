#include "sapper.h"

Sapper::Sapper(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	// ��������� ������� ��� �������������� ������
	QTextCodec *tc=QTextCodec::codecForName("CP1251");
	QTextCodec::setCodecForCStrings(tc);

	// �������� ��������� ��������� �����
	qsrand(time(NULL));

	// ���������� ����� ����������������� ����������
	QFile styleFile("style.css");
	styleFile.open(QFile::ReadOnly);
	QString qss = styleFile.readAll();
	qApp->setStyleSheet(qss);
    
	// ��� ����
	QPixmap background("img/background.gif");
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);
    this->setAutoFillBackground(true);

	// ��������� ����� �� ��������� ��� ����� ����������
	defaultInfo = ui.info->text();

	// ���������� ��������� ������� �� ������
	for (int i = 0; i < Sapper::height; i++)
	{
		for (int j = 0; j < Sapper::width; j++)
		{
			QPushButton * cell = this->findChild<QPushButton *>("cell" + QString::number(i) + QString::number(j));
			cell->installEventFilter(this);
		}
	}

	// ���������� ����� ����� ��������� � ������������� (�������)
	connect(&timer,				SIGNAL(timeout()), this, SLOT(secondGone()));
	connect(ui.newGame,			SIGNAL(clicked()), this, SLOT(startGame()));
	connect(ui.registration,	SIGNAL(clicked()), this, SLOT(showRegistrationForm()));
	connect(ui.login,			SIGNAL(clicked()), this, SLOT(authorization()));
	connect(ui.profile,			SIGNAL(clicked()), this, SLOT(showProfile()));

	// ���������������� ��������� ����
	registrationForm = new Registration(this);
    registrationForm->setWindowFlags(Qt::Dialog);
    registrationForm->setWindowModality(Qt::WindowModal);
	authorizationForm = new Authorization(this);
	authorizationForm->setWindowFlags(Qt::Dialog);
	authorizationForm->setWindowModality(Qt::WindowModal);
	
	// ��������� �������� ������� ������� �������
	ui.table->horizontalHeader()->setResizeMode(QHeaderView::Fixed);

	// ������� ����� - �����
	userId = -1;
	// ��� ����� �� ����� ������ "�������"
	ui.profile->setVisible(false);

	// ��������� ���� ������
	db.load();

	// ������ ����� ����
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

	// �������� ������ ������ ����
	for (int i = 0; i < Sapper::height; i++)
		for (int j = 0; j < Sapper::width; j++)
			minesField[i][j] = false;

	// ������������� ����� ����� ���
	curMineCount = minMineCount + qrand() % maxAddMineCount;

	// �������� ��������� ������� ���
	for (int i = 0; i < curMineCount; i++)
	{
		// ������������ ��������� ���������� �� ��� ���, ���� ��� �� ����� ��������� �� ��������� ������
		do 
		{
			x = qrand() % Sapper::height;
			y = qrand() % Sapper::width;
		} while (minesField[x][y] == true);

		// ���������� ����
		minesField[x][y] = true;
	}

	// ������� � ��������� ���� ���������� ���
	QString text = ui.info->text();
	text.replace(text.indexOf("<br/><br/>") + QString("<br/><br/>���������� ��� - ").length(), 2, QString::number(curMineCount));
	ui.info->setText(text);
}

void Sapper::startGame()
{
	// ������� ���� ���������� � ��������� ���������
	ui.info->setText(defaultInfo);

	// ���������� ����
	setMines();

	// �������� ������ ����� ����
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

	// ������� ����� ��� �� ����
	firstClick = false;

	// ���� ��������
	gameInProgress = true;

	// ���������� ������ (���� ������ ����� ����, �� �������� ������)
	timer.stop();

	// ������������� � ������� � �����
	ui.stackedWidget->setCurrentIndex(0);
}	

void Sapper::cellLeftClick(int x, int y)
{	
	// ������
	QPushButton * cell = this->findChild<QPushButton *>("cell" + QString::number(x) + QString::number(y));

	// ���� ������ ���� �������� - ����� ������
	if (fieldStatus[x][y] == MARKED)
	{
		fieldStatus[x][y] = CLOSED;

		// ������� ������
		cell->setStyleSheet("");
	}
	// ���� ������ ���� ������� - �������
	else if (fieldStatus[x][y] == CLOSED)
	{
		open(x, y);
	}
}

void Sapper::cellRightClick(int x, int y)
{
	// ������
	QPushButton * cell = this->findChild<QPushButton *>("cell" + QString::number(x) + QString::number(y));
	
	// ���� ������ ���� ������� - ���������� ������
	if (fieldStatus[x][y] == CLOSED)
	{
		fieldStatus[x][y] = MARKED;

		// �������� ������
		cell->setStyleSheet("background-image: url(img/red.png);");
	}
	// ���� ������ ���� �������� - ����� ������
	else if (fieldStatus[x][y] == MARKED)
	{
		fieldStatus[x][y] = CLOSED;

		// ������� ������
		cell->setStyleSheet("");
	}
}

void Sapper::open(int x, int y)
{
	// ��������� �� ����� ��������� �� ������� ����
	if (!inBorders(x, y))
		return;

	// ���� ������� ���� - ���������
	if (minesField[x][y] == true)
		finishGame(false);
	else
	{
		fieldStatus[x][y] = OPENED;
		closedCellCount--;
		// ������
		QPushButton * cell = this->findChild<QPushButton *>("cell" + QString::number(x) + QString::number(y));
		// ���������� ����� ���� ����
		cell->setStyleSheet("background-color: #fff;");

		// ���������� ���������� ���
		int count = mineCount(x, y);
		if (count == 0)
		{
			// ���� ��� ��� - ������� ��� �������� ������
			for (int i = -1; i <= 1; i++)
				for (int j = -1; j <= 1; j++)
					if (!(fieldStatus[x+i][y+j] == OPENED))
						open(x + i, y + j);
		}
		else
			cell->setText(QString::number(count));

		// ���� ���������� ����� ����� ����� ��� - �� ��������!!
		if (closedCellCount == curMineCount)
			finishGame(true);
	}
}

int Sapper::mineCount(int x, int y)
{
	int count = 0;

	// ��������� ���������� ��� � �������� 3�3 � ������� � ���������� ������
	for (int i = -1; i <= 1; i++)
		for (int j = -1; j <= 1; j++)
			if (inBorders(x + i, y + j) && minesField[x + i][y + j] == true)
				count++;

	return count;
}

void Sapper::finishGame(bool victory)
{
	// ���� ���� ��� ���� ��������� - ������ �� ������
	if (!gameInProgress)
		return;

	// ���� �����������
	gameInProgress = false;
	timer.stop();

	// �������� ��� ���� ��� ���������
	if (!victory)
	{
		for (int i = 0; i < Sapper::height; i++)
		{
			for (int j = 0; j < Sapper::width; j++)
			{
				if (minesField[i][j] == true)
				{
					// ������
					QPushButton * cell = this->findChild<QPushButton *>("cell" + QString::number(i) + QString::number(j));
					// ���������� ����� ���� ���� � ����������� ����
					cell->setStyleSheet("background-color: #fff;background-image: url(img/mine.png);");
				}
			}
		}
	}

	// ��������� ���� � �������
	GameInHistory gameInfo;
	gameInfo.minesCount = curMineCount;
	gameInfo.victory = victory;
	// �������� ����������� �����
	QString time = ui.info->text().right(8 + QString("</span></p></body></html>").length()).left(8);
	int s = time[6].digitValue() * 10 + time[7].digitValue();
	int m = time[3].digitValue() * 10 + time[4].digitValue();
	int h = time[0].digitValue() * 10 + time[1].digitValue();
	gameInfo.time = s + m * 60 + h * 3600;
	db.addGame(gameInfo, userId);

	// ������� ��������� ����
	ui.info->setText(ui.info->text() + "��������� - " + (victory ? "������" : "���������"));
}

bool Sapper::eventFilter(QObject *obj, QEvent *event)
{
	// ���� ������ ������� ���� �� ������ ����
    if (event->type() == QEvent::MouseButtonPress && obj->objectName().startsWith("cell"))
	{
		// ���� ���� �� ���� (�����������) - �� ����������� �� �������
		if (!gameInProgress)
			return true;

		// ��������� ���������� ������� ������
		int x = obj->objectName()[4].digitValue();
		int y = obj->objectName()[5].digitValue();

		QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
		// ��������� ������ ��� ������ ����� �� ����
		if ((mouseEvent->button() == Qt::LeftButton || mouseEvent->button() == Qt::RightButton) && !firstClick)
		{
			timer.setInterval(1000);
			timer.start();
			firstClick = true;
		}

		// ���� ������ ������ ������ ����
		if (mouseEvent->button() == Qt::RightButton)
			cellRightClick(x, y);
		// ���� ������ ����� ������ ����
		else if (mouseEvent->button() == Qt::LeftButton)
			cellLeftClick(x, y);

		return true;
	}
	// ����� ����������� ���������
	else
	{
		return QObject::eventFilter(obj, event);
	}
}

void Sapper::closeEvent(QCloseEvent *event)
{
	// ��������� ���� ������
	db.save();

	// ���������� ������
	delete registrationForm;
	delete authorizationForm;

	event->accept();
}

void Sapper::resizeEvent(QResizeEvent *event)
{
	int windowWidth = event->size().width();
	// ����������� ������ ����� � �����������
	ui.login->move(windowWidth - 5 - ui.login->width(), ui.login->pos().y());
	ui.registration->move(windowWidth - 6 - ui.login->width() - ui.registration->width(), ui.registration->pos().y());
	// ����������� ����� ������������
	ui.usernameLabel->move(windowWidth - 14 - ui.login->width() - ui.usernameLabel->width(), ui.usernameLabel->pos().y());
	// ��������� �������� �������
	ui.stackedWidget->resize(windowWidth, ui.stackedWidget->height());
	ui.whiteBg->resize(windowWidth - 80, ui.whiteBg->height());
	ui.table->resize(windowWidth - 120, ui.table->height());
	// ������ ������� ������� �������
	ui.table->setColumnWidth(0, ui.table->width() / 7);
	ui.table->setColumnWidth(1, ui.table->width() / 7 * 3);
	ui.table->setColumnWidth(2, ui.table->width() - (ui.table->width() / 7) * 4 - 2);
}

bool Sapper::registerUser(QString username, QString password)
{
	// �������� ������������ � ���� ������
	int id = db.addUser(username, password);
	// ���� �� ������� (��� ������������ ������) - ��������� �����������
	if (id == -1)
		return false;
	// ����� � ����� ���������� �������
	loginUser(username, password);
	return true;
}

bool Sapper::loginUser(QString username, QString password)
{
	// ��������� ��������������� ������ ������������
	int id = db.login(username, password);
	// ���� �� ������� �������������� - ��������� �����������
	if (id == -1)
		return false;
	
	userId = id;
		
	// ������� ��� ������������
	ui.usernameLabel->setText("�� ����� ��� " + db.username(userId));
	// ���������� ������ "�������"
	ui.profile->setVisible(true);
	// ������ ������ �����������
	ui.registration->setVisible(false);
	// ������ ����� ������ �����
	ui.login->setText("�����");

	return true;
}

void Sapper::secondGone()
{
	// �������� ������� ��������� �����
	QString time = ui.info->text().right(8 + QString("</span></p></body></html>").length()).left(8);
	int s = time[6].digitValue() * 10 + time[7].digitValue();
	int m = time[3].digitValue() * 10 + time[4].digitValue();
	int h = time[0].digitValue() * 10 + time[1].digitValue();

	// ��������� ���� �������
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
	
	// ���������� ����������� �����
	QString newTime = QString::number(h / 10) + QString::number(h % 10) + ":"
					+ QString::number(m / 10) + QString::number(m % 10) + ":"
					+ QString::number(s / 10) + QString::number(s % 10);
	QString newText = ui.info->text().replace(ui.info->text().length() - QString("</span></p></body></html>").length() - 8, 8, newTime);
	ui.info->setText(newText);

	// ������������� ������
	timer.start();
}

void Sapper::showRegistrationForm()
{
	registrationForm->clear();
	registrationForm->show();
}

void Sapper::authorization()
{
	// ���� ����� ��������������
	if (userId == -1)
	{
		// �������� ���� �����������
		authorizationForm->clear();
		authorizationForm->show();
	}
	// ���� ����� �����
	else
	{
		userId = -1;
		
		// �������� ���� � ������ ������������
		ui.usernameLabel->setText("");
		// ������ ������ "�������"
		ui.profile->setVisible(false);
		// ���������� ������ �����������
		ui.registration->setVisible(true);
		// ������ ������ ������ ����
		ui.login->setText("����");
		// ������������� � ������� � �����
		ui.stackedWidget->setCurrentIndex(0);
	}
}

void Sapper::showProfile()
{
	// ����� ���������
	ui.loseCount->setText("���������� ��������� - " + QString::number(db.loseCount(userId)));

	// ������
	QVector <GameInHistory> victories = db.victories(userId);
	ui.winCount->setText("�������� �������������� - " + QString::number(victories.size()));
	ui.table->setRowCount(victories.size());
	for (int i = 0; i < victories.size(); i++)
	{
		ui.table->setItem(i, 0, new QTableWidgetItem(QString::number(victories[i].number)));
		ui.table->setItem(i, 1, new QTableWidgetItem(QString::number(victories[i].minesCount)));
		ui.table->setItem(i, 2, new QTableWidgetItem(QString::number(victories[i].time)));
	}

	// ������������� � ������� �������
	ui.stackedWidget->setCurrentIndex(1);
}