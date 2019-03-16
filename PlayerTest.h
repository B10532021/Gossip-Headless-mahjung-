#pragma once
#include"MJCards.h"
#include"Player.h"

class PlayerTest : public Player {
public:
	PlayerTest();
	MJCard Throw();
	std::pair<CommandType, MJCard> WannaHuGon(bool canHu, bool canGon, const MJCard & card, const MJCard & gonCard);
	CommandType WannaHGPE(bool canHu, bool canGon, bool canPon, bool canEat, const MJCard & card, int idx);

	int Pick2Eat(const MJCard & card);
};