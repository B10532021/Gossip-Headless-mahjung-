#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>


typedef unsigned int Tiles;
typedef unsigned short Feature;
#define SIZE 0444444444

class HuTable
{
public:
	friend std::ostream& operator<<(std::ostream&, const HuTable&);
	Feature operator[](Tiles) const;
protected:
	HuTable();
	
	std::vector<int> melds, allEyes, allSingle;
	Feature *preTable;
	std::unordered_map<Tiles, Feature> table;

	virtual void init() = 0;
	bool have(Tiles now, Tiles check);
	bool canHu(const Tiles&);
	void filter() {}
};




//---------------SimplesTable------------------------------
class SimplesTable : public HuTable
{
public:
	SimplesTable();
	Feature operator[](Tiles) const;
	std::string toString(const Feature&);

private:
	enum feature
	{
		CAN_HU            = 1,      // bit0: 可胡
		HAS_EYE           = 2,      // bit1: 有眼
		ONLY_HAS_EYE      = 4,      // bit2: 只有眼
		TIN_ONLY_ONE      = 8,      // bit3: 獨聽
		CLEAN             = 16,     // bit4: 門清
		PING_HU           = 32,     // bit5: 平胡
		MIX_SUIT          = 64,     // bit6: 湊一色
		SAME_SUIT         = 128,    // bit7: 清一色
		PON_PAIR          = 256,    // bit8: 碰碰胡
		CONCEALED_PON_CNT = 01000,  // bit9-11: 暗刻數
		EYE_NUMBER        = 0xf000, // bit12-15: 眼睛位置
	};

private:
	void init();
	void setCanHu(int sets, Tiles curr, Tiles all);
	void setOnlyHasEye();
	void setPingHu(int sets, Tiles curr, Tiles c);
	void setSameSuit();
	void setPonPair(int sets, Tiles curr, Tiles c);
	void setConcealedPonCnt();
	void setEyeNumber();
	void setTinOnlyOne();

	void filter();
};




//---------------HonorsTable------------------------------
class HonorsTable : public HuTable
{
public:
	HonorsTable();
	
private:
	enum feature
	{
		CAN_HU              = 1,     // bit0: 可胡
		HAS_EYE             = 2,     // bit1: 有眼
		ONLY_HAS_EYE        = 4,     // bit2: 只有眼
		TIN_ONLY_ONE        = 8,     // bit3: 獨聽
		EAST                = 16,    // bit4: 東風
		SOUTH               = 32,    // bit5: 南風
		WEST                = 64,    // bit6: 西風
		NORTH               = 128,   // bit7: 北風
		BONUS               = 256,   // bit8: 1
		RED                 = 512,   // bit9: 紅中
		GREEN               = 1024,  // bit10: 青發
		WHITE               = 2048,  // bit11: 白板
		LITTLE_FOUR_WINDS   = 4096,  // 小四喜
		BIG_FOUR_WINDS      = 8192,  // 大四喜
		LITTE_THREE_DRAGONS = 16384, // 小三元
		BIG_THREE_DRAGONS   = 32768, // 大三元
	};

private:
	void init();
	void setCanHu(int sets, int curr, int all);
	void setWindsAndDragons();
	void setTinOnlyOne();
	void filter();
};