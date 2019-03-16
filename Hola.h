#pragma once
#include "MJCards.h"
#include "Player.h"
#include "irb01.h"
#include <iostream>
#include <map>
#include <vector>
#include <string>
using namespace std;
struct Times
{
	int times;
	int continuous;
	void clear();
	Times() :times(0), continuous(0) {}
	~Times() {}
};


class Hola
{
private:
	vector<int> GetComponent(Tiles_ T);
	void FindGroups(Tiles_ T, const vector<int>& Componet, vector<Groups>& result, vector<int> tmp, int start);
	vector<Groups> CountSteps(vector<Groups>& G);
	bool Have(int t, int m);

public:
	int uid;
	static int duration;//第幾手
	int move;//0~2：無、摸牌、打牌
	int happen;//0~14：無、吃、碰、槓、碰槓、暗槓、進牌、摸到前三輪丟出去的牌、打危險牌、跟打、摸到無用的牌、摸到安全牌、聽牌、自摸、胡、流局、摸到六張花牌以上
	bool take_action;
	bool can_eat;
	bool last_card;
	bool keepwin;
	//bool safe;
	Times times[4];//0~3：吃、碰、槓、聽牌後沒摸到聽得牌
	Times ba_times[3];//0~2：被吃、被碰、被槓
	bool tin;
	bool other_tin;//別人聽牌
	vector<MJCard> throw_before;
	vector<MJCard> safe_before;
	std::string Print(std::map<string, std::vector<std::string>> & conditions);
	std::string Print2();
	void clear();
	void all_clear();
	void Happen(CommandType HGPE);
	void BaHappen(CommandType HGPE);
	void CantEat();
	void AddSelfBefore(MJCard card);
	bool CheckIsUseless(const MJCard & card, const MJCards & Table);
	bool CheckLastNeed(Player player, CommandType HGPE, const MJCard & card, const MJCards & Discards);//哈拉麻將吃到最後一張牌
	bool CheckIsSafe(const MJCard & card, const MJCards & Discards);//是否打安全牌
	void CheckKeepWin(bool keepWin);
	Tiles_ HandToTiles(const MJCards & cards);
	Tiles_ HandToTiles(const MJCards & cards, const MJCard & card);
	int StepToHu1617(const Tiles_ & Hand);
	//std::string Condition(std::map<int, std::vector<std::string>> & conditions);
	Hola();
	~Hola();
};

