#include "MCTSPlayer.h"
#include "GameManager.h"
#include <cstdio>

MCTSPlayer::MCTSPlayer(const int maxTimes, const double maxTimeInSec, const int explorationParameter) :
	Player(), maxTimes(maxTimes), maxTimeInSec(maxTimeInSec), explorationParameter(explorationParameter) {
	std::srand(std::time(NULL));
}

MJCard MCTSPlayer::Throw(const MJCard &card)
{
	cout << "Throw" << endl;
	auto decision = MakeDecision(0, MJCard(-1, -1), false);
	//assert(decision.first == NONE);
	return decision.second;
}

std::pair<CommandType, MJCard> MCTSPlayer::WannaHuGon(bool canHu, bool canGon, const MJCard &card, const MJCard & gonCard)
{
	cout << "WannaHuGon: " <<  (canHu ? "canZimo, " : "") << (canGon ? "canAnGon, " : "") << "card = (" << card.color << ", " << card.value << ")" << endl;
	auto decision = MakeDecision(0, card, true);
	if (decision.first == COMMAND_ANGON || decision.first == COMMAND_PONGON)
		decision.first = COMMAND_GON;
	//assert(decision.first == COMMAND_ZIMO && canHu || decision.first == COMMAND_ANGON && canGon || decision.first == NONE);
	if (!(decision.first == COMMAND_ZIMO && canHu || decision.first == COMMAND_GON && canGon || decision.first == NONE)) {
		cout << "Error in WannaHuGon: decision.first = " << decision.first << ", canHu = " << canHu << ", canGon = " << canGon <<
			", card = (" << card.color << ", " << card.value << ")" << endl;
		system("pause");
	}
	return decision;
}

CommandType MCTSPlayer::WannaHGPE(bool canHu, bool canGon, bool canPon, bool canEat, const MJCard &card, int idx)
{
	int player = (idx - id + 4) % 4;
	cout << "WannaHGPE: " <<  (canHu ? "canHu, " : "") << (canGon ? "canGon, " : "") << (canPon ? "canPon, " : "") << (canEat ? "canEat, " : "") <<
		"player = " << player << ", card = (" << card.color << ", " << card.value << ")" << endl;
	//assert(0 <= player && player <= 3);
	auto decision = MakeDecision(player, card, true);
	//assert(decision.first == COMMAND_HU && canHu || decision.first == COMMAND_GON && canGon || decision.first == COMMAND_PON && canPon || decision.first == COMMAND_EAT && canEat || decision.first == NONE);
	if (!(decision.first == COMMAND_HU && canHu || decision.first == COMMAND_GON && canGon || decision.first == COMMAND_PON && canPon || decision.first == COMMAND_EAT && canEat || decision.first == NONE)) {
		cout << "Error in WannaHGPE: decision.first = " << decision.first << ", canHu = " << canHu << ", canGon = " << canGon << ", canPon = " << canPon <<
			", canEat = " << canEat << ", card = (" << card.color << ", " << card.value << ")" << endl;
		system("pause");
	}
	return decision.first;
}

int MCTSPlayer::Pick2Eat(const MJCard &card)
{
	return eatType;
}

std::pair<CommandType, MJCard> MCTSPlayer::MakeDecision(const int playerIdx, const MJCard &actionCard, const bool addToHand)
{
	// left = all - discarded - hand - door - others' (doors - anGons)
	// left = all - Discards - hand - anGons
	MJCards hand(Hand), door(false), left(true);
	if (addToHand)
		hand += actionCard;
	(left -= *discards) -= hand;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 7; j++) {
			int temp;
			temp = (eat >> (i * 21 + j * 3)) & 7;
			for (int k = 0; k < temp; k++) {
				door.mjcolor[i].value += (73 << (j * 3));
			}
		}
	}
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			int temp;
			temp = (pon >> (i * 9 + j)) & 1;
			if (temp)
				door.mjcolor[i].value += (3 << j * 3);
			temp = (gon >> (i * 9 + j)) & 1;
			if (temp)
				door.mjcolor[i].value += (4 << j * 3);
			temp = (anGon >> (i * 9 + j)) & 1;
			if (temp) {
				door.mjcolor[i].value += (4 << j * 3);
				left.mjcolor[i].value -= (4 << j * 3);
			}
		}
	}
	for (int j = 0; j < 4; j++) {
		int temp;
		temp = (pon >> (30 + j)) & 1;
		if (temp)
			door.windsCards += (3 << j * 3);
		temp = (gon >> (30 + j)) & 1;
		if (temp)
			door.windsCards += (4 << j * 3);
		temp = (anGon >> (30 + j)) & 1;
		if (temp) {
			door.windsCards += (4 << j * 3);
			left.windsCards -= (4 << j * 3);
		}
	}
	for (int j = 0; j < 3; j++) {
		int temp;
		temp = (pon >> (27 + j)) & 1;
		if (temp)
			door.cfbCards += (3 << j * 3);
		temp = (gon >> (27 + j)) & 1;
		if (temp)
			door.cfbCards += (4 << j * 3);
		temp = (anGon >> (27 + j)) & 1;
		if (temp) {
			door.cfbCards += (4 << j * 3);
			left.cfbCards -= (4 << j * 3);
		}
	}
	door.flowersCards |= flowers;

	MCTS mcts(playerIdx, hand, left, door, GameManager::gameManager->Sea.Count() - 16, actionCard, maxTimes, maxTimeInSec, explorationParameter);
	Node *best = mcts.MakeDecision();
	CommandType action = NONE;

	int diff;
	auto [color, value, number] = hand.Find([&](int color, int value, int number) {
		MJCard card(color, value);
		return (diff = number - best->hand.Count(card)) != 0;
	});
	MJCard card(color, value);
	switch (diff) {
	case 1: // 少1張
		if (best->isLeaf_value > 0) { // 自摸/胡
			action = playerIdx == 0 ? COMMAND_ZIMO : COMMAND_HU;
		}
		else if (playerIdx != 0 && best->door[color][value] - door[color][value] == 1) { // 吃
			action = COMMAND_EAT;
			card = actionCard;
			color = actionCard.color;
			value = actionCard.value;
			if (value >= 2) {
				if (hand[color][value - 2] - best->hand[color][value - 2] == 1) {
					eatType = 0;
					break;
				}
			}
			if (value <= 6) {
				if (hand[color][value + 2] - best->hand[color][value + 2] == 1) {
					eatType = 2;
					break;
				}
			}
			eatType = 1;
		}
		else if (best->door[color][value] == 4) { // 加槓
			action = COMMAND_PONGON;
		}
		else { // PASS/丟牌
			action = NONE;
		}
		break;
	case 3: // 少3張：碰
		action = COMMAND_PON;
		break;
	case 4: // 少4張
		if (playerIdx == 0)
			action = COMMAND_ANGON; // 暗槓
		else
			action = COMMAND_GON; // 明槓
		break;
	}
	return std::pair<CommandType, MJCard>(action, card);
}