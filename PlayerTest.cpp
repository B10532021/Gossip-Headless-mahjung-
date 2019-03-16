#include"PlayerTest.h"


MJCard PlayerTest::Throw() {

	return Hand.Rand();
}

PlayerTest::PlayerTest() {
	Action = THROW;
}

std::pair<CommandType, MJCard> PlayerTest::WannaHuGon(bool canHu, bool canGon, const MJCard & card, const MJCard & gonCard) {
	if (canHu)
		return std::make_pair(COMMAND_ZIMO, card);
	else if (canGon)
		return std::make_pair(COMMAND_GON, gonCard); 
	else
		return std::make_pair(NONE, Hand.Rand());
}

CommandType PlayerTest::WannaHGPE(bool canHu, bool canGon, bool canPon, bool canEat, const MJCard & card, int idx) {
	if (canHu) {
		return COMMAND_HU;
	}
	else if (canGon) {
		return COMMAND_GON;
	}
	else if (canPon) {
		return COMMAND_PON;
	}
	else if (canEat) {
		return COMMAND_EAT;
	}
	else {
		cout << "WannaHGPE error" << endl;
		return NONE;
	}
}

int PlayerTest::Pick2Eat(const MJCard & card) {
	if (card.color > 2 || card.color == -1)
	{
		cout << "pick2eat error" << endl;
		return -1;
	}

	//¬Ý¬O§_¬O 1 9
	if (card.value == 0) {
		return 2;
	}
	else if (card.value == 8) {
		return 0;
	}

	//2 8
	else if (card.value == 1) {
		if (Hand.Count(MJCard(card.color, card.value - 1)) >= 1 && Hand.Count(MJCard(card.color, card.value + 1)) >= 1)
			return 1;
		else
			return 2;
	}
	else if (card.value == 7) {
		if (Hand.Count(MJCard(card.color, card.value - 1)) >= 1 && Hand.Count(MJCard(card.color, card.value + 1)) >= 1)
			return 1;
		else
			return 0;
		//if (players[index]->Hand.Count(MJCard(card.color, card.value - 1)) >= 1 && players[index]->Hand.Count(MJCard(card.color, card.value - 2)) >= 1)
			//return true;
	}


	else {
		if (Hand.Count(MJCard(card.color, card.value - 1)) >= 1 && Hand.Count(MJCard(card.color, card.value + 1)) >= 1)
			return 1;
		if (Hand.Count(MJCard(card.color, card.value + 1)) >= 1 && Hand.Count(MJCard(card.color, card.value + 2)) >= 1)
			return 2;
		if (Hand.Count(MJCard(card.color, card.value - 1)) >= 1 && Hand.Count(MJCard(card.color, card.value - 2)) >= 1)
			return 0;
	}

	return -1;
}