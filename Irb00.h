
#pragma once
#include"MJCards.h"
#include"Player.h"

class Irb00 : public Player {
public:
	Irb00();
	MJCard Throw(const MJCard & card = MJCard(-100, -100));
	std::pair<CommandType, MJCard> WannaHuGon(bool canHu, bool canGon, const MJCard & card, const MJCard & gonCard);
	CommandType WannaHGPE(bool canHu, bool canGon, bool canPon, bool canEat, const MJCard & card, int idx);

	int Pick2Eat(const MJCard & card);
};