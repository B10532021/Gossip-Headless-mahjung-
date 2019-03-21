#pragma once

#include"MJCards.h"
#include"Player.h"
#include<map>
#include<time.h>
#include"PlayerTest.h"
#include"Irb00.h"
#include"Irb01.h"
#include <ctime>
#include <chrono>
#include "ScoreCalc.h"
#include <memory>
#include "Hola.h"
using namespace std;

class GameManager;
struct PlayerCardsInfo;
class Player;
class Hola;

struct Record{//�O���C����P����T
	int turn;
	int uid;
	MJCard card;
	Record(int t, int id, const MJCard& c) :turn(t), uid(id)
	{
		this->card = c;
	}
} ;

class GameManager
{
public:
	GameManager();
	static Player * players[4];
	static MJCards Sea;
	Hola features[4];
	int play_round;//�ĴX��
	int currentIdx;
	int dronIdx; //���aidx
	int turn;
	int winds;
	int score[4] = { 0, 0, 0, 0 }; //��uid�����A�Hscore[uid]�覡�O��
	int uid[4] = { 0, 0, 0, 0 }; //����players[0]~players[3] ��uid
	bool moPie;
	bool gameOver = false;
	vector<Record> throwRecord;//�O���C���P����T

	bool tie = false;
	bool canHu, canPon, canEat;
	pair<bool, MJCard> canGon;
	MJCards Discards, HuCards;
	pair<CommandType, MJCard> action = make_pair(NONE, MJCard(-1, -1));
	CommandType HGPE[3];
	void Init();
	void Run(int rounds, std::string fileName);
	static GameManager* gameManager;
	static void InitPlayer();
	void SetSeat();
	void SetTable();
	void BuHua();
	void DrawCard();
	void ThrowCard();
	void AnyActions(const MJCard & card);
	bool CheckGonHu(const MJCard & card, int currentIdx); //�ˬd�m�b�J
	bool Check7Huas(int currentIdx);
	int FlowersCount(int flowers);
	bool sevenHuas; //�ˬd���S���H�w�g���C�i��F
	int sevenId; //���C�i�᪺�H��ID
	bool keepWin; //�s��
	int numKeepWin; //�s����
	
	
	bool CheckHu(int idx, const MJCard & card);
	pair<bool, MJCard> CheckGon(int idx, const MJCard & card, bool myDraw = false);
	bool CheckPon(int idx, const MJCard & card);
	bool CheckEat(int idx, const MJCard & card);
	long long CardTo34(const MJCard & card);
	long long CardTo63(const MJCard & card); //�ⶶ�l(�Y)���Ĥ@�i�P��i�h�A���ͬ۹�����long long eat����ƫ��A
	long long CardTo63Order(const MJCard & card, int middle);

	void EatConvert(const long long & eat);
	void PonConvert(const long long & pon);
	void GonConvert(const long long & gon, bool min = true);
	void CertainTileConvert(const MJCard & card);

	void PrintTai(int playeridx, const MJCard & card, bool zimo, int wind, int numKeepWin, int pao, bool gonHu = false, bool lastCard = false, bool sevenHua = false); //pao���񬶪̪�idx //gonHu�ˬd�m�b�J�[�@�x //lastCard ��������[�@�x //sevenHua�C�m�@
	
private:
	class Log;
	class CsvLog;
	std::unique_ptr<Log> log;
};



class GameManager::Log
{
public:
	enum State {
		DRAW,
		DISCARD,
		REPLACE,
		CHOW,
		PONG,
		KONG,
		ADD_KONG,
		CONCEALED_KONG,
		HU,
		ZIMO,
	};

public:
	Log(GameManager*, const std::string& = "default.log");

	virtual void logStart(const int& play_round, const int& round, const int& door) = 0;
	virtual void logState(const int& turn, const State&, const MJCard&,
		const int& actionPlayer, const int& targetPlayer = -1) = 0;
	virtual void logResult(const short&, const short&, const std::string&) = 0;
	virtual void writeLog(const std::string&) = 0;
	virtual void logConversation(const int& turn, const std::string&) = 0;

	virtual ~Log() = default;

protected:
	Log() = default;
	GameManager* gameManager;
	std::ofstream fout;

	const char* C2W = "ABCDEF";
};

class GameManager::CsvLog : public GameManager::Log
{
public:
	CsvLog(GameManager*, const std::string& = "default.csv");

	void logStart(const int& play_round, const int& round, const int& door);
	void logState(const int& turn, const State&, const MJCard&,
		const int& actionPlayer, const int& targetPlayer = -1) override;
	void logResult(const short&, const short&, const std::string&) override;
	void writeLog(const std::string&) override;
	void logConversation(const int& turn, const std::string &);

	~CsvLog() override;
};
