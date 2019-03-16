#include "Hola.h"

static const int GroupMask[40] = { 02,020,0200,02000,020000,0200000,02000000,020000000,0200000000,\
03,030,0300,03000,030000,0300000,03000000,030000000,0300000000,\
0111,01110,011100,0111000,01110000,011100000,0111000000,\
011,0110,01100,011000,0110000,01100000,011000000,0110000000,\
0101,01010,010100,0101000,01010000,010100000,0101000000 };

void Times::clear()
{
	times = 0;
	continuous = 0;
}

int Hola::StepToHu1617(const Tiles_ & Hand) {
	vector<int> component = GetComponent(Hand);

	vector<Groups> result;
	vector<int> tmp;
	FindGroups(Hand, component, result, tmp, 0);

	vector<Groups> G = CountSteps(result);

	return G[0].steps;
}

vector<int> Hola::GetComponent(Tiles_ T)
{
	vector<int> ret;
	for (int i = 0; i < 138; i++)
	{
		if (Have(T.tiles[i / 40], GroupMask[i % 40]))
			ret.push_back(i);
	}
	return ret;
}

void Hola::FindGroups(Tiles_ T, const vector<int>& Componet, vector<Groups>& result, vector<int> tmp, int start) {
	bool f = true;
	for (int i = start; i < Componet.size(); i++)
	{
		if (Have(T.tiles[Componet[i] / 40], GroupMask[Componet[i] % 40]))
		{
			f = false;
			Tiles_ t = T;
			t.tiles[Componet[i] / 40] -= GroupMask[Componet[i] % 40];
			tmp.push_back(Componet[i]);
			FindGroups(t, Componet, result, tmp, i);
			tmp.pop_back();
		}
	}

	if (f)
	{
		for (int i = 0; i < 34; i++)
		{
			if ((T.tiles[i / 9] >> (i % 9 * 3)) & 7)
			{
				for (int j = 0; j < ((T.tiles[i / 9] >> (i % 9 * 3)) & 7); j++)
					tmp.push_back(180 + i);
			}
		}
		Groups G;
		G.code = tmp;
		result.push_back(G);
	}
}

vector<Groups> Hola::CountSteps(vector<Groups>& G) {
	vector<Groups> ret;

	int min = 100;
	int total, single, pair, group, pre_group, steps;
	for (int i = 0; i < G.size(); i++)
	{
		single = 0, pair = 0, group = 0, pre_group = 0;
		for (int j = 0; j < G[i].code.size(); j++)
		{
			if (G[i].code[j] >= 180)
				single++;
			else if (G[i].code[j] % 40 < 9)
				pair++;
			else if (G[i].code[j] % 40 < 25)
				group++;
			else
				pre_group++;
		}
		total = group * 3 + pre_group * 2 + pair * 2 + single;
		steps = total / 3 * 2 + 1;
		steps -= group * 2 + pre_group + pair;

		if (group + pre_group + pair > total / 3 + 1)
			steps += group + pre_group + pair - (total / 3 + 1);

		if (pre_group != 0 && pre_group >= (total / 3 + 1 - group) && !pair)
			steps += 1;

		if (steps < min)
			min = steps;

		G[i].steps = steps;
	}

	for (int i = 0; i < G.size(); i++)
	{
		if (G[i].steps <= min + 0)
			ret.push_back(G[i]);
	}

	return ret;
}

Tiles_ Hola::HandToTiles(const MJCards & cards) {

	Tiles_ ret;
	ret.tiles[0] = cards.mjcolor[0].value;
	ret.tiles[1] = cards.mjcolor[1].value;
	ret.tiles[2] = cards.mjcolor[2].value;
	ret.tiles[3] = cards.cfbCards << 12;
	ret.tiles[3] += cards.windsCards;

	return ret;

}

Tiles_ Hola::HandToTiles(const MJCards & cards, const MJCard & card) {

	Tiles_ ret;
	ret.tiles[0] = cards.mjcolor[0].value;
	ret.tiles[1] = cards.mjcolor[1].value;
	ret.tiles[2] = cards.mjcolor[2].value;
	ret.tiles[3] = cards.cfbCards << 12;
	ret.tiles[3] += cards.windsCards;

	if (card.color >= 0 && card.color <= 2) {
		ret.tiles[card.color] += 1 << (card.value * 3);
	}
	else if (card.color == 3) {
		ret.tiles[3] += 1 << (card.value * 3 + 12);

	}
	else if (card.color == 4) {
		ret.tiles[3] += 1 << (card.value * 3);

	}
	else {
		cout << "HandToTiles Error" << endl;
	}

	return ret;

}

bool Hola::Have(int t, int m)
{
	for (int i = 0; i < 9; i++)
		if (((t >> i * 3) & 7) < ((m >> i * 3) & 7))
			return false;
	return true;
}

Hola::Hola()
{
	move = 0;
	happen = 0;
	take_action = 1;
	can_eat = 1;
	last_card = 0;
	tin = 0;
	other_tin = 0;
	keepwin = 0;
	for (int i = 0; i < 4; i++)
	{
		times[i].clear();
	}
	for (int i = 0; i < 3; i++)
	{
		ba_times[i].clear();
	}
}

void Hola::clear()
{
	move = 0;
	happen = 0;
	take_action = 1;
	can_eat = 1;
	last_card = 0;
}

void Hola::all_clear() {
	move = 0;
	happen = 0;
	take_action = 1;
	can_eat = 1;
	last_card = 0;
	tin = 0;
	other_tin = 0;
	for (int i = 0; i < 4; i++)
	{
		times[i].clear();
	}
	for (int i = 0; i < 3; i++)
	{
		ba_times[i].clear();
	}
	throw_before.clear();
	safe_before.clear();
}

string Hola::Print(map<string, vector<string>> & conditions)
{
	string str = "";
	/*if ((happen == 8 || happen == 9) && move == 2)
	{
		str += "," + to_string((uid + rand() % 3 + 1) % 4) + ",��,";
	}
	else */
	if (happen != 0 || !take_action)
	{
		str += "," + to_string(uid) + ",��,";
	}

	int pos = 0;
	switch (happen)
	{
	case 0://�S�Ʊ�
		if (!take_action)
		{
			pos = 1;
		}
		break;
	case 1://�Y
		if (move == 2)
		{
			pos = 19;
			if (ba_times[0].continuous > 1)
			{
				pos = 20;
			}
			else if (ba_times[0].times > 2)
			{
				pos = 21;
			}
		}
		else
		{
			if (can_eat)
			{
				pos = 2;
				if (last_card)
				{
					pos = 3;
				}
				else if (times[0].continuous > 1)
				{
					pos = 4;
				}
				else if (times[0].times > 2)
				{
					pos = 5;
				}
			}
			else
			{
				if (!last_card)
				{
					pos = 6;
				}
				else
				{
					pos = 7;
				}
			}
		}
		break;
	case 2://�I
		if (move == 2)
		{
			pos = 22;
			if (ba_times[1].continuous > 1)
			{
				pos = 23;
			}
			else if (ba_times[0].times > 2)
			{
				pos = 24;
			}
		}
		else
		{
			pos = 8;
			if (last_card)
			{
				pos = 9;
			}
			else if (times[1].continuous > 1)
			{
				pos = 10;
			}
			else if (times[1].times > 2)
			{
				if (times[0].times == 0 && times[1].times > 3)
				{
					pos = 12;
				}
				else
				{
					pos = 11;
				}
			}
		}
		break;
	case 3://�b
		if (move == 2)
		{
			pos = 25;
			if (ba_times[2].continuous > 1)
			{
				pos = 26;
			}
			else if (ba_times[2].times > 1)
			{
				pos = 27;
			}
		}
		else
		{
			pos = 13;
			if (times[2].continuous > 1)
			{
				pos = 14;
			}
			else if (times[2].times > 1)
			{
				pos = 15;
			}
		}
		break;
	case 4://�I�b
		pos = 16;
		break;
	case 5://�t�b
		pos = 17;
		break;
	case 6://�i�P
		pos = 33;
		break;
	case 7://�N��e�T����X�h���P
		pos = 34;
		break;
	case 8://���M�I�P
		if (move == 2)
			pos = 29;
		else
			pos = 30;
		break;
	case 9://��
		if (move == 2)
			pos = 31;
		else
			pos = 32;
		break;
	case 10://�N��L�Ϊ��P
		pos = 35;
		break;
	case 11://�N��w���P
		pos = 36;
		break;
	case 12://ť�P
		if (duration >= 15)//ť�P
		{
			pos = 37;
		}
		else//�W��ť�P
		{
			pos = 38;
		}
		break;
	case 13://�ۺN
		pos = 39;
		break;
	case 14://�J�P
		if (move == 2)//�Q�J
		{
			pos = 41;
		}
		else//�J�P
		{
			pos = 40;
		}
		break;
	case 15://�y��
		pos = 42;
		break;
	case 16://�s��
		pos = 43;
		break;
	case 17://�N�W�L���i��H�W
		pos = 44;
		break;
	default:
		break;
	}


	if (happen > 0 && happen < 4)
	{
		int count = 0;
		for (int i = 0; i < 3; i++)
		{
			if (move == 2)
			{
				count += ba_times[i].times;
			}
			else
			{
				count += times[i].times;
			}
		}
		if (move == 2 && count > 4)
		{
			pos == 28;
		}
		else if (move == 0 && count > 3 && pos != 12)
		{
			pos = 18;
		}
	}
	string position = to_string(pos);
	if (pos != 0 && conditions.find(position) != conditions.end())
	{
		int random = rand() % conditions[position].size();
		str += conditions[position][random] + ",,,,,,,,,,,,";
	}

	return str;
}

string Hola::Print2()
{
	string str = "";
	if (happen != 0)
	{
		str += ",�o��,";
	}
	int temp = 0;
	switch (happen)
	{
	case 0://�S�Ʊ�
		if (!take_action)
		{
			str += "�i�H�Y�I�b�����Ĩ����";
		}
		break;
	case 1://�Y
		if (move == 2)
		{
			str += "���P�B�Q�Y";
			if (ba_times[0].continuous > 1)
			{
				str += "�B�Q�s�Y" + to_string(ba_times[0].continuous) + "��";
			}
			else if (ba_times[0].times > 2)
			{
				str += "�F��" + to_string(ba_times[0].times) + "��";
			}
		}
		else
		{
			if (can_eat)
			{
				if (last_card)
				{
					str += "�Y��̫�@�i�ݭn���P";
				}
				else
				{
					str += "�Y";
					if (times[0].continuous > 1)
					{
						str += "�B�s�Y" + to_string(times[0].continuous) + "��";
					}
					else if (times[0].times > 2)
					{
						str += "�F��" + to_string(times[0].times) + "��";
					}
				}

			}
			else
			{
				if (last_card)
				{
					str += "�Y����̫�@�i�ݭn���P";
				}
				else
				{
					str += "�Y����";
				}
			}
		}
		break;
	case 2://�I
		if (move == 2)
		{
			str += "���P�B�Q�I";
			if (ba_times[1].continuous > 1)
			{
				str += "�B�Q�s�I" + to_string(ba_times[1].continuous) + "��";
			}
			else if (ba_times[0].times > 2)
			{
				str += "�F��" + to_string(ba_times[1].times) + "��";
			}
		}
		else
		{
			if (last_card)
			{
				str += "�I��̫�@�i�P";
			}
			else
			{
				str += "�I";
				if (times[1].continuous > 1)
				{
					str += "�B�s�I" + to_string(times[1].continuous) + "��";
				}
				else if (times[1].times > 2)
				{
					if (times[0].times == 0 && times[1].times > 3)
					{
						str += "�F��" + to_string(times[1].times) + "���B0�Y";
						temp = 1;
					}
					else
					{
						str += "�F��" + to_string(times[1].times) + "��";
					}
				}
			}
		}
		break;
	case 3://�b
		if (move == 2)
		{
			str += "���P�B�Q�b";
			if (ba_times[2].continuous > 1)
			{
				str += "�B�Q�s�Y" + to_string(ba_times[2].continuous) + "��";
			}
			else if (ba_times[2].times > 1)
			{
				str += "�F��" + to_string(ba_times[2].times) + "��";
			}
		}
		else
		{
			str += "�b";
			if (times[2].continuous > 1)
			{
				str += "�B�s�b" + to_string(times[1].continuous) + "��";
			}
			else if (times[2].times > 1)
			{
				str += "�F��" + to_string(times[1].times) + "��";
			}
		}
		break;
	case 4://�I�b
		str += "�N�P�B�I�b";
		break;
	case 5://�t�b
		str += "�N�P�B�t�P";
		break;
	case 6://�i�P
		str += "�N�P�B�i�P";
		break;
	case 7://�N��e�T����X�h���P
		str += "�N�P�B�N��e�T����L���P";
		break;
	case 8://���M�I�P
		str += "���P�B���M�I�P";
		break;
	case 9://��
		str += "���P�B��";
		break;
	case 10:
		str += "�N�P�B�N��L�Ϊ��P";
		break;
	case 11:
		str += "�N�P�B�N��w���P";
		break;
	case 12:
		if (duration < 15)
		{
			str += "�W��ť�P";
		}
		else
		{
			str += "ť�P";
		}

		break;
	case 13:
		str += "�ۺN";
		break;
	case 14:
		if (move == 2)
		{
			str += "�Q�J�P";
		}
		else
		{
			str += "�J�P";
		}
		break;
	case 15://�y��
		str += "�y��";
		break;
	case 16:
		str += "�s��";
		break;
	case 17://�ɤ��i��H�W
		str += "�ɤ��i��H�W";
		break;
	default:
		break;
	}

	if (happen > 0 && happen < 4)
	{
		int count = 0;
		for (int i = 0; i < 3; i++)
		{
			if (move == 2)
			{
				count += ba_times[i].times;
			}
			else
			{
				count += times[i].times;
			}
		}
		if (move == 2 && count > 4)
		{
			str += "�B�e�F" + to_string(count) + "�P";
		}
		else if (move == 0 && count > 3 && temp != 1)
		{
			str += "���e��" + to_string(count) + "��";
		}
	}
	if (happen != 0)
	{
		str += ",,,,,,,,,,,,";
	}
	return str;
}

void Hola::Happen(CommandType HGPE)
{
	if (HGPE == COMMAND_GON)//���Գ±N
	{
		this->happen = 3;
		this->times[2].continuous++;
		this->times[0].continuous = 0;
		this->times[1].continuous = 0;
	}
	else if (HGPE == COMMAND_PON)
	{
		this->happen = 2;
		this->times[1].continuous++;
		this->times[0].continuous = 0;
		this->times[2].continuous = 0;
	}
	else if (HGPE == COMMAND_EAT)
	{
		this->happen = 1;
		this->times[0].continuous++;
		this->times[1].continuous = 0;
		this->times[2].continuous = 0;
	}
	else if (HGPE == NONE)
	{
		this->times[0].continuous = 0;
		this->times[1].continuous = 0;
		this->times[2].continuous = 0;
	}
}

void Hola::BaHappen(CommandType HGPE)
{
	if (HGPE == COMMAND_GON)//���Գ±N
	{
		this->happen = 3;
		this->ba_times[2].times++;
		this->ba_times[2].continuous++;
		this->ba_times[0].continuous = 0;
		this->ba_times[1].continuous = 0;
	}
	else if (HGPE == COMMAND_PON)
	{
		this->happen = 2;
		this->ba_times[1].times++;
		this->ba_times[1].continuous++;
		this->ba_times[0].continuous = 0;
		this->ba_times[2].continuous = 0;
	}
	else if (HGPE == COMMAND_EAT)
	{
		this->happen = 1;//���Գ±N
		this->ba_times[0].times++;
		this->ba_times[0].continuous++;
		this->ba_times[1].continuous = 0;
		this->ba_times[2].continuous = 0;
	}
	else if (HGPE == NONE)
	{
		this->happen = 0;
		this->ba_times[0].continuous = 0;
		this->ba_times[1].continuous = 0;
		this->ba_times[2].continuous = 0;
	}
}

void Hola::CantEat()
{
	this->can_eat = 0;
	this->times[0].times--;
	this->times[0].continuous--;
}

void Hola::AddSelfBefore(MJCard card)
{
	this->throw_before.push_back(card);
	if (this->throw_before.size() > 4)
	{
		this->throw_before.erase(this->throw_before.begin());
	}
}

bool Hola::CheckIsUseless(const MJCard & card, const MJCards & Table)
{
	bool is_useless = false;
	if (card.color >= 0 && card.color <= 2) {//�U����
		if (Table.mjcolor[card.color][card.value] >= 2) {
			is_useless = true;
		}
	}
	else if (card.color == 3) {//�r�P
		if (((Table.cfbCards >> (card.value * 3)) & 7) >= 2) {
			is_useless = true;
		}
	}
	else if (card.color == 4) {//���P
		if (((Table.windsCards >> (card.value * 3)) & 7) >= 2) {
			is_useless = true;
		}
	}
	return is_useless;
}

Hola::~Hola()
{
	//delete [] times;
	//delete [] ba_times;
	//delete [] throw_before;
	//delete[] safe_before;
}

bool Hola::CheckLastNeed(Player player, CommandType HGPE, const MJCard & card, const MJCards & Discards)//���Գ±N
{
	bool is_the_last = false;
	if (HGPE == COMMAND_EAT)
	{
		if ((Discards.mjcolor[card.color][card.value] + player.Hand.mjcolor[card.color][card.value]) == 4) {
			is_the_last = true;
		}
	}
	else if (HGPE == COMMAND_PON)
	{
		if (card.color >= 0 && card.color <= 2) {//�U����
			if ((Discards.mjcolor[card.color][card.value] + player.Hand.mjcolor[card.color][card.value]) == 4) {
				is_the_last = true;
			}
		}
		else if (card.color == 3) {//�r�P
			if (((Discards.cfbCards >> (card.value * 3)) & 7) + (player.Hand.cfbCards >> (card.value * 3) & 7) == 4) {
				is_the_last = true;
			}
		}
		else if (card.color == 4) {//���P
			if (((Discards.windsCards >> (card.value * 3)) & 7) + (player.Hand.windsCards >> (card.value * 3) & 7) == 4) {
				is_the_last = true;
			}
		}
	}
	else if (HGPE == NONE)//�N��κN�P��
	{

	}

	return is_the_last;
}

bool Hola::CheckIsSafe(const MJCard & card, const MJCards & Discards)
{
	bool is_safe = true;
	if (card.color >= 0 && card.color <= 2) {//�U����
		if (Discards.mjcolor[card.color][card.value] <= 2) {
			is_safe = false;
		}
	}
	else if (card.color == 3) {//�r�P
		if (((Discards.cfbCards >> (card.value * 3)) & 7) <= 2) {
			is_safe = false;
		}
	}
	else if (card.color == 4) {//���P
		if (((Discards.windsCards >> (card.value * 3)) & 7) <= 2) {
			is_safe = false;
		}
	}
	return is_safe;
}

void Hola::CheckKeepWin(bool keepWin)
{
	if (keepWin) {
		keepwin = true;
	}
	else {
		keepwin = false;
	}
}

