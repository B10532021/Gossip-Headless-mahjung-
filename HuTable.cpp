#include "HuTable.h"
#include <algorithm>

Feature HuTable::operator[](Tiles idx) const
{
	const auto it = table.find(idx);
	if (it == table.end())
		return 0;
	else
		return it->second;
}

HuTable::HuTable()
{
	// 0
	melds.push_back(0);
	// ��l
	for (size_t i = 0; i < 9; i++)
		melds.push_back(3 << i * 3);
	// ���l
	for (size_t i = 0; i < 7; i++)
		melds.push_back(0111 << i * 3);
	// ��
	for (size_t i = 0; i < 9; i++)
		allEyes.push_back(2 << i * 3);

	for (size_t i = 0; i < 9; i++)
		allSingle.push_back(1 << i * 3);
}

// �ˬd��P�O�_�]�t�S�w���P��
// now����P check���n�ˬd���P��
// �Y��P���Areturn true 
bool HuTable::have(Tiles now, Tiles check)
{
	for (int i = 0; i < 9; i++)
		if (((now >> i * 3) & 7) < ((check >> i * 3) & 7))
			return false;
	return true;
}

bool HuTable::canHu(const Tiles& t)
{
	return (preTable[t & 0777777777] & 1);
}

std::ostream& operator<<(std::ostream& out, const HuTable& huTable)
{
	for (const auto& m : huTable.table)
		out << m.first << " " << m.second << std::endl;
	return out;
}




//---------------SimplesTable------------------------------
SimplesTable::SimplesTable()
{
	init();
}

Feature SimplesTable::operator[](Tiles idx) const
{
	auto it = table.find(idx);
	return it != table.end() ? it->second : 0;
}

// for debug
std::string SimplesTable::toString(const Feature& f)
{
	std::string result;
	if (f& CAN_HU)
		result += "�i�J";
	if (f& HAS_EYE)
		result += "����";
	if (f& ONLY_HAS_EYE)
		result += "�u����";
	if (f& TIN_ONLY_ONE)
		result += "�Wť";
	if (f& CLEAN)
		result += "���M";
	if (f& PING_HU)
		result += "���J";
	if (f& MIX_SUIT)
		result += "��@��";
	if (f& SAME_SUIT)
		result += "�M�@��";
	if (f& PON_PAIR)
		result += "�I�I�J";
	result += std::to_string((f >> 9) & 7) + "�t��";
	if (f >> 12)
		result += "����" + std::to_string(f >> 12) + "\n";
	return result;
}

void SimplesTable::init()
{
	preTable = new unsigned short[SIZE];

	setCanHu(5, 0, SIZE);
	setOnlyHasEye();
	setPingHu(5, 0, SIZE);
	setSameSuit();
	setPonPair(5, 0, 1);
	setConcealedPonCnt();
	setEyeNumber();
	setTinOnlyOne();

	filter();
	delete[] preTable;
}

void SimplesTable::setCanHu(int sets, Tiles curr, Tiles all)
{
	if (sets) {
		for (int i = 0; i < 17; i++)
			if (have(all, melds[i]))
				setCanHu(sets - 1, curr + melds[i], all - melds[i]);
	}
	else {
		preTable[curr] |= CAN_HU;
		//����
		for (int i = 0; i < 9; i++)
			if (have(all, allEyes[i]))
				preTable[curr + allEyes[i]] |= (HAS_EYE | CAN_HU);
	}
}

void SimplesTable::setOnlyHasEye()
{
	preTable[0] |= ONLY_HAS_EYE;
	for (size_t i = 0; i < 9; i++)
		preTable[allEyes[i]] |= ONLY_HAS_EYE;
}

void SimplesTable::setPingHu(int sets, Tiles curr, Tiles all)
{
	if (sets) {
		for (int i = 0; i < 17; i++)
			if (have(all, melds[i]) && (i == 0 || i > 9))
				setPingHu(sets - 1, curr + melds[i], all - melds[i]);
	}
	else {
		preTable[curr] |= PING_HU;
		//����
		for (int i = 0; i < 9; i++)
			if (have(all, allEyes[i]))
				preTable[curr + allEyes[i]] |= PING_HU;
	}
}

void SimplesTable::setSameSuit()
{
	preTable[0] |= SAME_SUIT;
}

void SimplesTable::setPonPair(int sets, Tiles curr, Tiles c)
{
	preTable[curr] |= PON_PAIR;
	for (size_t i = 0; i < 9; i++)
		if (have(SIZE - curr, allEyes[i]))
			preTable[curr + allEyes[i]] |= PON_PAIR;

	if (sets)
		for (int i = c; i <= 9; i++)
			setPonPair(sets - 1, curr + melds[i], i + 1);
}

void SimplesTable::setConcealedPonCnt()
{
	for (int i = 0; i < SIZE; i++)
	{
		int ponCnt = 0;
		if (canHu(i)) {
			for (int j = 0; j < 9; j++) {
				if (have(i, melds[j + 1]))
					if (canHu(i - melds[j + 1]))
						ponCnt++;
			}
			preTable[i] |= (std::min(ponCnt, 7) * CONCEALED_PON_CNT);
		}
	}
}

void SimplesTable::setEyeNumber()
{
	for (int i = 0; i < SIZE; i++)
	{
		int pos = 0;
		if (canHu(i)) {
			for (int j = 0; j < 9; j++) {
				if (have(i, allEyes[j]))
					if (canHu(i - allEyes[j]))
						pos = j + 1;
			}
			preTable[i] |= (pos << 12);
		}
	}
}

void SimplesTable::setTinOnlyOne()
{
	for (Tiles tiles = 1; tiles < SIZE; tiles++)
	{
		if (canHu(tiles))
		{
			for (Tiles j = 0; j < 9; j++)
			{
				if (have(tiles, allSingle[j]))
				{
					Feature huFeature = preTable[tiles], zimoFeature = preTable[tiles];
					Tiles readyHand = tiles - allSingle[j];
					int tinCnt = 0;

					for (int k = 0; k < 9; k++)
					{
						auto temp = readyHand + allSingle[k];
						if (temp > SIZE)
							continue;
						if (canHu(temp))
							tinCnt++;
					}
					// �J��l�A�D�ۺN�t���-1�A�L�Wť
					if (have(tiles, melds[j + 1]))
						if (canHu(tiles - melds[j + 1]))
						{
							huFeature -= (1 << 9);
							tinCnt++;
						}
					// �Wť
					if (tinCnt == 1)
					{
						huFeature |= TIN_ONLY_ONE;
						zimoFeature |= TIN_ONLY_ONE;
					}
					// ���p���J(�J���A�Wť)
					if (((preTable[tiles] >> 12) & 0b1111) == j + 1 || tinCnt == 1)
						huFeature &= ~PING_HU;

					table[((j + 1) << 27) | tiles] = huFeature;
					table[(1 << 31) | ((j + 1) << 27) | tiles] = zimoFeature;
				}
			}
		}
	}
}

void SimplesTable::filter()
{
	for (int i = 0; i < SIZE; i++)
		if (canHu(i))
			table[i] = preTable[i];
}




//---------------HonorsTable------------------------------
HonorsTable::HonorsTable()
{
	init();
}

void HonorsTable::init()
{
	preTable = new unsigned short[SIZE];
	setCanHu(5, 0, SIZE);
	setWindsAndDragons();
	setTinOnlyOne();
	filter();
	delete[] preTable;
}

void HonorsTable::setCanHu(int sets, int curr, int all)
{
	if (sets)
	{
		for (int i = 0; i < 8; i++)
			if (have(all, melds[i]))
				setCanHu(sets - 1, curr + melds[i], all - melds[i]);
	}
	else
	{
		preTable[curr] |= (BONUS | CAN_HU);
		//����
		for (int i = 0; i < 7; i++)
			if (have(all, allEyes[i]))
				preTable[curr + allEyes[i]] |= (BONUS | HAS_EYE | CAN_HU);
	}
}

void HonorsTable::setWindsAndDragons()
{
	for (Tiles i = 0; i < SIZE; i++)
	{
		const Feature w[7] = { EAST, SOUTH, WEST, NORTH, RED, GREEN, WHITE };
		if (preTable[i] & 1)
		{
			for (size_t j = 0; j < 7; j++) {
				if (have(i, melds[j + 1]))
					preTable[i] |= w[j];
			}

			auto winds = i & 07777;
			auto dragons = i & 07770000;
			auto clearWinds = ~(EAST | SOUTH | WEST | NORTH);
			auto clearDragons = ~(RED | GREEN | WHITE);

			// �p�|��
			if (winds == 03332 || winds == 03323 || winds == 03233 || winds == 02333)
				preTable[i] = (preTable[i] | LITTLE_FOUR_WINDS) & clearWinds;
			// �j�|��
			else if (winds == 03333)
				preTable[i] = (preTable[i] | BIG_FOUR_WINDS) & clearWinds;
			// �p�T��
			else if (dragons == 03320000 || dragons == 03230000 || dragons == 02330000)
				preTable[i] = (preTable[i] | LITTE_THREE_DRAGONS) & clearDragons;
			// �j�T��
			else if (dragons == 03330000)
				preTable[i] = (preTable[i] | BIG_THREE_DRAGONS) & clearDragons;
		}
	}
}

void HonorsTable::setTinOnlyOne()
{
	for (Tiles tiles = 1; tiles < SIZE; tiles++)
	{
		if (canHu(tiles))
		{
			for (Tiles j = 0; j < 7; j++)
			{
				if (have(tiles, allSingle[j]))
				{
					Feature huFeature = preTable[tiles], zimoFeature = preTable[tiles];
					Tiles tilesOfTin = tiles - allSingle[j];
					int tinCnt = 0;

					for (int k = 0; k < 9; k++)
					{
						auto temp = tilesOfTin + allSingle[k];
						if (temp > SIZE)
							continue;
						if (canHu(temp))
							tinCnt++;
					}
					// �J��l�A�D�ۺN�t���-1�A�L�Wť
					if (have(tiles, melds[j + 1]))
						if (canHu(tiles - melds[j + 1]))
						{

							tinCnt++;
						}
					// �Wť
					if (tinCnt == 1)
					{
						huFeature |= TIN_ONLY_ONE;
						zimoFeature |= TIN_ONLY_ONE;
					}

					//if(temp != 0)
					table[((j + 1) << 27) | tiles] = huFeature;
					table[(1 << 31) | ((j + 1) << 27) | tiles] = zimoFeature;
				}
			}
		}
	}
}

void HonorsTable::filter()
{
	for (int i = 0; i < SIZE; i++)
		if (canHu(i))
			table[i] = preTable[i];
}
