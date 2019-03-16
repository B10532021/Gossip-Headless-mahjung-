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
		CAN_HU            = 1,      // bit0: �i�J
		HAS_EYE           = 2,      // bit1: ����
		ONLY_HAS_EYE      = 4,      // bit2: �u����
		TIN_ONLY_ONE      = 8,      // bit3: �Wť
		CLEAN             = 16,     // bit4: ���M
		PING_HU           = 32,     // bit5: ���J
		MIX_SUIT          = 64,     // bit6: ��@��
		SAME_SUIT         = 128,    // bit7: �M�@��
		PON_PAIR          = 256,    // bit8: �I�I�J
		CONCEALED_PON_CNT = 01000,  // bit9-11: �t���
		EYE_NUMBER        = 0xf000, // bit12-15: ������m
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
		CAN_HU              = 1,     // bit0: �i�J
		HAS_EYE             = 2,     // bit1: ����
		ONLY_HAS_EYE        = 4,     // bit2: �u����
		TIN_ONLY_ONE        = 8,     // bit3: �Wť
		EAST                = 16,    // bit4: �F��
		SOUTH               = 32,    // bit5: �n��
		WEST                = 64,    // bit6: �護
		NORTH               = 128,   // bit7: �_��
		BONUS               = 256,   // bit8: 1
		RED                 = 512,   // bit9: ����
		GREEN               = 1024,  // bit10: �C�o
		WHITE               = 2048,  // bit11: �ժO
		LITTLE_FOUR_WINDS   = 4096,  // �p�|��
		BIG_FOUR_WINDS      = 8192,  // �j�|��
		LITTE_THREE_DRAGONS = 16384, // �p�T��
		BIG_THREE_DRAGONS   = 32768, // �j�T��
	};

private:
	void init();
	void setCanHu(int sets, int curr, int all);
	void setWindsAndDragons();
	void setTinOnlyOne();
	void filter();
};