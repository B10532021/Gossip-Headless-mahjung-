#pragma once
#include "HuTable.h"
typedef unsigned int FeatureAll;

class SimplesTaiTable
{
public:
	SimplesTaiTable();
	std::pair<short, std::string> operator[](Feature) const;

	friend std::ostream& operator<<(std::ostream&, const SimplesTaiTable&);

private:
	std::unordered_map<Feature, std::pair<short, std::string>> taiTable; // map<Feature, pair<台數, 胡牌排型>
	enum feature
	{
		CAN_HU            = 1,      // bit0: 可胡
		HAS_EYE           = 2,      // bit1: 有眼
		ONLY_HAS_EYE      = 4,      // bit2: 全求人
		TIN_ONLY_ONE      = 8,      // bit3: 獨聽
		CLEAN             = 16,     // bit4: 門清
		PING_HU           = 32,     // bit5: 平胡
		MIX_SUIT          = 64,     // bit6: 湊一色
		SAME_SUIT         = 128,    // bit7: 清一色
		PON_PAIR          = 256,    // bit8: 碰碰胡
		CONCEALED_PON_CNT = 01000,  // bit9-11: 三四五暗刻
		ZIMO              = 010000, // bit12:自摸
	};

private:
	void initTable();

// for debug
private:
	void test();
};


class HonorsTaiTable
{
public:
	HonorsTaiTable();

	std::pair<short, std::string> operator[] (Feature) const;
	friend std::ostream& operator<<(std::ostream&, const SimplesTaiTable&);
private:
	std::unordered_map<Feature, std::pair<short, std::string>> taiTable; // map<Feature, pair<台數, 胡牌排型>
	enum feature
	{
		
		EAST                = 1,    // bit0: 東風
		SOUTH               = 2,    // bit1: 南風
		WEST                = 4,    // bit2: 西風
		NORTH               = 8,    // bit3: 北風
		BONUS               = 16,   // bit4: 圈風門風一樣
		RED                 = 32,   // bit5: 紅中
		GREEN               = 64,   // bit6: 青發
		WHITE               = 128,  // bit7: 白板
		LITTLE_FOUR_WINDS   = 256,  // 小四喜
		BIG_FOUR_WINDS      = 512,  // 大四喜
		LITTE_THREE_DRAGONS = 1024, // 小三元
		BIG_THREE_DRAGONS   = 2048, // 大三元
	};

private:
	void initTable();
};



class TaiTable
{
public:
	TaiTable() = default;
	std::pair<short, std::string> operator[](FeatureAll) const;

private:
	SimplesTaiTable sTable;
	HonorsTaiTable hTable;
};
