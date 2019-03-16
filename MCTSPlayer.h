#pragma once
#include "GameManager.h"
#include "Player.h"
#include "MCTS.h"

class MCTSPlayer : public Player {
public:
	MCTSPlayer(const int maxTimes = DEFAULT_MAX_TIMES, const double maxTimeInSec = DEFAULT_TIME_LIMIT, const int explorationParameter = DEFAULT_EXPLORATION_PARAMETER);

	MJCard Throw(const MJCard & card = MJCard(-100, -100)) override;
	std::pair<CommandType, MJCard> WannaHuGon(bool canHu, bool canGon, const MJCard & card, const MJCard & gonCard) override;
	CommandType WannaHGPE(bool canHu, bool canGon, bool canPon, bool canEat, const MJCard & card, int idx) override;
	int Pick2Eat(const MJCard & card) override;

	std::pair<CommandType, MJCard> MakeDecision(const int playerIdx, const MJCard &actionCard, const bool addToHand = false);

	const int maxTimes;
	const double maxTimeInSec;
	const int explorationParameter;

private:
	size_t eatType; //0代表用左邊兩張吃 1代表中洞吃 2代表右邊兩張吃
	//MJCard decidedThrow;
};