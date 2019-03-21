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

struct Record{//記錄每次丟牌的資訊
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
	int play_round;//第幾局
	int currentIdx;
	int dronIdx; //莊家idx
	int turn;
	int winds;
	int score[4] = { 0, 0, 0, 0 }; //用uid紀錄，以score[uid]方式記錄
	int uid[4] = { 0, 0, 0, 0 }; //紀錄players[0]~players[3] 的uid
	bool moPie;
	bool gameOver = false;
	vector<Record> throwRecord;//記錄每手丟牌的資訊

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
	bool CheckGonHu(const MJCard & card, int currentIdx); //檢查搶槓胡
	bool Check7Huas(int currentIdx);
	int FlowersCount(int flowers);
	bool sevenHuas; //檢查有沒有人已經有七張花了
	int sevenId; //有七張花的人的ID
	bool keepWin; //連莊
	int numKeepWin; //連莊數
	
	
	bool CheckHu(int idx, const MJCard & card);
	pair<bool, MJCard> CheckGon(int idx, const MJCard & card, bool myDraw = false);
	bool CheckPon(int idx, const MJCard & card);
	bool CheckEat(int idx, const MJCard & card);
	long long CardTo34(const MJCard & card);
	long long CardTo63(const MJCard & card); //把順子(吃)的第一張牌丟進去，產生相對應的long long eat的資料型態
	long long CardTo63Order(const MJCard & card, int middle);

	void EatConvert(const long long & eat);
	void PonConvert(const long long & pon);
	void GonConvert(const long long & gon, bool min = true);
	void CertainTileConvert(const MJCard & card);

	void PrintTai(int playeridx, const MJCard & card, bool zimo, int wind, int numKeepWin, int pao, bool gonHu = false, bool lastCard = false, bool sevenHua = false); //pao為放炮者的idx //gonHu檢查搶槓胡加一台 //lastCard 海底撈月加一台 //sevenHua七搶一
	
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
