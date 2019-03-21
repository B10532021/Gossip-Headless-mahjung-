#include"Player.h"

static const char* C2W = "ABCDEF";


std::string Player::Print() {
	std::string str;

	str += ",Hand," + Hand.Print();
	
	str += ",Eat,";
	/*for (int i = 0; i < 21; i++) {
		for (int k = 0; k < ((eat >> (i * 3)) & 7); k++)
		{
			str += to_string(i) + " ";
		}
	}*/
	for (int j = 0; j < 3; j++) {
		for (int i = 0; i < 21; i++) {
			for (int k = 0; k < ((eatOrder[j] >> (i * 3)) & 7); k++)
			{
				str += to_string(i + j * 21) + " ";
			}
		}
	}

	str += ",Pon,";
	for (int i = 0; i < 34; i++) {
		if ((pon >> i) & 1) {
			str += to_string(i) + " ";
		}
	}
	str += ",Gon,";
	for (int i = 0; i < 34; i++) {
		if ((gon >> i) & 1) {
			str += to_string(i) + " ";
		}
	}
	
	str += ",AnGon,";
	for (int i = 0; i < 34; i++) {
		if ((anGon >> i) & 1) {
			str += to_string(i) + " ";
		}
	}

	str += ",Hua,";
	for (int i = 0; i < 8; i++) {
		if ((flowers >> i) & 1) {
			str += to_string(i) + " ";
		}
	}

	return str;
}

Player::Player() {
	eat = pon = gon = anGon = flowers = 0;
}

int Player::Count() {
	int result = 0;
	result += Hand.Count();
	/*for (int i = 0; i < 21; i++) {
		result += ((eat >> (i * 3)) & 7) * 3;
	}*/
	for (int j = 0; j < 3; j++) {
		for (int i = 0; i < 21; i++) {
			result += ((eatOrder[j] >> (i * 3)) & 7) * 3;
		}
	}
	for (int i = 0; i < 34; i++) {
		result += ((pon >> i) & 1) * 3;
	}
	for (int i = 0; i < 34; i++) {
		result += ((gon >> i) & 1) * 3;
	}
	for (int i = 0; i < 34; i++) {
		result += ((anGon >> i) & 1) * 3;
	}

	return result;
}



bool Player::Ready() {
	return 1;
}

bool Player::IsHu() {
	return false;
}

MJCard Player::Throw(const MJCard & card) {
	return Hand.Rand();
}

std::pair<CommandType, MJCard> Player::WannaHuGon(bool canHu, bool canGon, const MJCard & card, const MJCard & gonCard) {
	if (canHu)
		return std::make_pair(COMMAND_ZIMO, MJCard(-1, -1));
	else if (canGon)
		return std::make_pair(COMMAND_GON, MJCard(-1,-1)); //¡Ÿ®S¶“º{∑t∫b
	else
		return std::make_pair(NONE, Hand.Rand());
}

bool Player::WannaHu(const MJCard & card) {
	return true;
}

CommandType Player::WannaHGPE(bool canHu, bool canGon, bool canPon, bool canEat, const MJCard & card, int idx) {
	return NONE;
}

int Player::Pick2Eat(const MJCard & card) {
	return 0;
}