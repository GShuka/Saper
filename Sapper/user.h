#include "GameInHistory.h"
#include <QVector>
#include <QDataStream>

class User
{
public:
	User();

	int id;								// �������������
	QString username;					// ���
	QString password;					// ������
	int loseCount;						// ���������� ���������
	QVector <GameInHistory> victories;	// ������ (������������� �� ������������)

	// �������� ��������� ���� � ������������
	void addGame(GameInHistory game);
};

// ����� � ����
QDataStream &operator << (QDataStream &out, const User &cur);

// ���� �� �����
QDataStream &operator >> (QDataStream &in, User &cur);