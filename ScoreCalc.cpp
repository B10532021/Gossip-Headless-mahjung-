#include "ScoreCalc.h"
#include "HuTable.h"
#include <iostream>
#include <bitset>


ScoreCalc& ScoreCalc::getInstance()
{
	static ScoreCalc scoreCalc;
	return scoreCalc;
}

// 回傳台數和牌型，若無法胡牌則回傳-1
std::pair<short, std::string> ScoreCalc::countTai(HuInfo info)
{
	const auto &simples = *simplesTable;
	const auto &honors = *honorsTable;
	auto &hands = info.hands;
	const auto &doors = info.doors;

	hands[info.certainTile / 9] |= (info.isZimo << 31 | (info.certainTile % 9 + 1) << 27);
	Feature f = 0;
	const auto c = simples[hands[0]]; // 萬
	const auto d = simples[hands[1]]; // 筒
	const auto b = simples[hands[2]]; // 條
	const auto h = simples[hands[3]]; // 字

	//   bit3   bit2  bit1   bit0
    // |  字  |  條  |  筒  |  萬  |
	const std::bitset<4> suits = (!!(hands[3] + doors[3]) << 3) | (!!(hands[2] + doors[2]) << 2) | 
		(!!(hands[1] + doors[1]) << 1) | !!(hands[0] + doors[0]); 
													 					
	auto honorsFeature = honors[hands[3] + kongToPong(doors[3])]; // 門前+手上字牌(槓轉為碰)
	
	// 無法胡牌
	if (!((c & d & b & honorsFeature) & CAN_HU) ||
		((c & HAS_EYE) + (d & HAS_EYE) + (b & HAS_EYE) + (honorsFeature & HAS_EYE)) != HAS_EYE) {
		return std::make_pair(-1, "");
	}

	const auto onlyHasEye = (c & d & b & h) & ONLY_HAS_EYE;
	const auto tinOnlyOne = (c | d | b | honorsFeature) & TIN_ONLY_ONE;
	const auto concealedPonCnt = ((c & CONCEALED_PON_CNT) + (d & CONCEALED_PON_CNT) +
		(b & CONCEALED_PON_CNT) + (h & CONCEALED_PON_CNT));
	f |= concealedPonCnt | tinOnlyOne | onlyHasEye | HAS_EYE | CAN_HU;

	//平胡(門前全為吃牌且無字無花)
	if (info.allChow&& info.noBonus && (hands[3] + doors[3]) == 0)
		f |= ((c & d & b & h) & PING_HU);

	// 湊一色
	if (suits == 0b1001 || suits == 0b1010 || suits == 0b1100)
		f |= MIX_SUIT;
	// 清一色
	else if (suits == 0b0001 || suits == 0b0010 || suits == 0b0100 || suits == 0b1000)
		f |= SAME_SUIT;

	// 碰碰胡
	if (info.allPon)
		f |= ((c & d & b) & PON_PAIR);
	
	// 門清
	if((doors[0] + doors[1] + doors[2] + doors[3]) == 0 || info.isClean)
		f |= CLEAN;
	// 自摸
	f |= info.isZimo ? ZIMO : 0;
	
	// 判斷門風和圈風
	honorsFeature &= ((info.winds.all | ~0b11111) << 4);
	return (*taiTable)[(honorsFeature << 16) | f];
}

std::string ScoreCalc::huInfoToString(const HuInfo& info)
{
	const std::string suits[4] = { "萬","筒","條","字" };
	const std::string honors[7] = { "東","南","西","北","中","發","白" };

	std::string result;

	std::string going;
	if (info.certainTile < 0 || info.certainTile > 34)
		return "error!\n";

	if (info.certainTile / 9 == 3)
		going = honors[info.certainTile % 9];
	else
		going = std::to_string(info.certainTile % 9 + 1) + suits[info.certainTile / 9];

	for (size_t i = 0; i < 3; i++) {
		for (size_t j = 0; j < 9; j++) {
			unsigned int cnt = (info.hands[i] >> j * 3) & 0b111;
			if (cnt > 4)
				return "Error!\n";

			if (i * 9 + j == info.certainTile) cnt--;

			for (size_t k = 0; k < cnt; k++)
				result += std::to_string(j + 1) + suits[i];
		}
		result += " ";
	}

	for (size_t i = 0; i < 9; i++) {
		int cnt = (info.hands[3] >> i * 3) & 0b111;
		if (cnt > 4)
			return "Error!\n";
		if (27 + i == info.certainTile) cnt--;
		for (size_t k = 0; k < cnt; k++)
			result += honors[i];
	}
	result += " ";

	result += going + " ";
	if (info.isZimo)
		result += " 自摸";
	else
		result += " 胡";
	return result;
}

ScoreCalc::ScoreCalc()
{
	simplesTable = std::make_unique<SimplesTable>();
	honorsTable = std::make_unique<HonorsTable>();
	taiTable = std::make_unique<TaiTable>();

	std::cout << "Table Finished!" << std::endl;
}