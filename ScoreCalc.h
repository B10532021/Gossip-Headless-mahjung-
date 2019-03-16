#pragma once
#include "TaiTable.h"
#include "HuTable.h"
#include <string>
#include <memory>
typedef unsigned int uint;

struct HuInfo
{
	Tiles hands[4];
	Tiles doors[4];
	union Winds
	{
		struct {
			char e : 1; // 東
			char s : 1; // 南
			char w : 1; // 西
			char n : 1; // 北
			char same : 1; // 門風圈風相同
		}_;
		char all : 5;
	} winds;

	bool isZimo = true;
	bool isClean = true;
	bool allPon = true;
	bool allChow = true;
	bool noBonus = true;

	char certainTile;
};

class ScoreCalc
{
public:
	static ScoreCalc& getInstance();
	std::pair<short, std::string> countTai(HuInfo);
	static std::string huInfoToString(const HuInfo&);
	
private:
	ScoreCalc();
	std::unique_ptr<TaiTable> taiTable;
	std::unique_ptr<HuTable> simplesTable;
	std::unique_ptr<HuTable> honorsTable;
	
	enum feature
	{
		CAN_HU = 1,   // bit0: 可胡
		HAS_EYE = 2,   // bit1: 有眼
		ONLY_HAS_EYE = 4,   // bit2: 只有眼
		TIN_ONLY_ONE = 8,   // bit3: 獨聽
		CLEAN = 16,   // bit4: 門清
		PING_HU = 32,   // bit5: 平胡
		MIX_SUIT = 64,   // bit6: 湊一色
		SAME_SUIT = 128,   // bit7: 清一色
		PON_PAIR = 256,   // bit8: 碰碰胡
		CONCEALED_PON_CNT = 07000, // bit9-11: 暗刻數
		ZIMO = 010000,
	};

private:
	Tiles kongToPong(Tiles t) {
		for (size_t i = 0; i < 9; i++) {
			auto cnt = (t >> i * 3) & 7;
			if (cnt == 4)
				t -= 1 << i * 3;
		}
		return t;
	}
};

