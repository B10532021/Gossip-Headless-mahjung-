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
	static int duration;//�ĴX��
	int move;//0~2�G�L�B�N�P�B���P
	int happen;//0~14�G�L�B�Y�B�I�B�b�B�I�b�B�t�b�B�i�P�B�N��e�T����X�h���P�B���M�I�P�B�򥴡B�N��L�Ϊ��P�B�N��w���P�Bť�P�B�ۺN�B�J�B�y���B�N�줻�i��P�H�W
	bool take_action;
	bool can_eat;
	bool last_card;
	bool keepwin;
	//bool safe;
	Times times[4];//0~3�G�Y�B�I�B�b�Bť�P��S�N��ť�o�P
	Times ba_times[3];//0~2�G�Q�Y�B�Q�I�B�Q�b
	bool tin;
	bool other_tin;//�O�Hť�P
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
	bool CheckLastNeed(Player player, CommandType HGPE, const MJCard & card, const MJCards & Discards);//���Գ±N�Y��̫�@�i�P
	bool CheckIsSafe(const MJCard & card, const MJCards & Discards);//�O�_���w���P
	void CheckKeepWin(bool keepWin);
	Tiles_ HandToTiles(const MJCards & cards);
	Tiles_ HandToTiles(const MJCards & cards, const MJCard & card);
	int StepToHu1617(const Tiles_ & Hand);
	//std::string Condition(std::map<int, std::vector<std::string>> & conditions);
	Hola();
	~Hola();
};

