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
		str += "," + to_string((uid + rand() % 3 + 1) % 4) + ",說,";
	}
	else */
	if (happen != 0 || !take_action)
	{
		str += "," + to_string(uid) + ",說,";
	}

	int pos = 0;
	switch (happen)
	{
	case 0://沒事情
		if (!take_action)
		{
			pos = 1;
		}
		break;
	case 1://吃
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
	case 2://碰
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
	case 3://槓
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
	case 4://碰槓
		pos = 16;
		break;
	case 5://暗槓
		pos = 17;
		break;
	case 6://進牌
		pos = 33;
		break;
	case 7://摸到前三輪丟出去的牌
		pos = 34;
		break;
	case 8://打危險牌
		if (move == 2)
			pos = 29;
		else
			pos = 30;
		break;
	case 9://跟打
		if (move == 2)
			pos = 31;
		else
			pos = 32;
		break;
	case 10://摸到無用的牌
		pos = 35;
		break;
	case 11://摸到安全牌
		pos = 36;
		break;
	case 12://聽牌
		if (duration >= 15)//聽牌
		{
			pos = 37;
		}
		else//超快聽牌
		{
			pos = 38;
		}
		break;
	case 13://自摸
		pos = 39;
		break;
	case 14://胡牌
		if (move == 2)//被胡
		{
			pos = 41;
		}
		else//胡牌
		{
			pos = 40;
		}
		break;
	case 15://流局
		pos = 42;
		break;
	case 16://連莊
		pos = 43;
		break;
	case 17://摸超過六張花以上
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
		str += ",發生,";
	}
	int temp = 0;
	switch (happen)
	{
	case 0://沒事情
		if (!take_action)
		{
			str += "可以吃碰槓但不採取行動";
		}
		break;
	case 1://吃
		if (move == 2)
		{
			str += "打牌、被吃";
			if (ba_times[0].continuous > 1)
			{
				str += "、被連吃" + to_string(ba_times[0].continuous) + "次";
			}
			else if (ba_times[0].times > 2)
			{
				str += "了第" + to_string(ba_times[0].times) + "次";
			}
		}
		else
		{
			if (can_eat)
			{
				if (last_card)
				{
					str += "吃到最後一張需要的牌";
				}
				else
				{
					str += "吃";
					if (times[0].continuous > 1)
					{
						str += "、連吃" + to_string(times[0].continuous) + "次";
					}
					else if (times[0].times > 2)
					{
						str += "了第" + to_string(times[0].times) + "次";
					}
				}

			}
			else
			{
				if (last_card)
				{
					str += "吃不到最後一張需要的牌";
				}
				else
				{
					str += "吃不到";
				}
			}
		}
		break;
	case 2://碰
		if (move == 2)
		{
			str += "打牌、被碰";
			if (ba_times[1].continuous > 1)
			{
				str += "、被連碰" + to_string(ba_times[1].continuous) + "次";
			}
			else if (ba_times[0].times > 2)
			{
				str += "了第" + to_string(ba_times[1].times) + "次";
			}
		}
		else
		{
			if (last_card)
			{
				str += "碰到最後一張牌";
			}
			else
			{
				str += "碰";
				if (times[1].continuous > 1)
				{
					str += "、連碰" + to_string(times[1].continuous) + "次";
				}
				else if (times[1].times > 2)
				{
					if (times[0].times == 0 && times[1].times > 3)
					{
						str += "了第" + to_string(times[1].times) + "次、0吃";
						temp = 1;
					}
					else
					{
						str += "了第" + to_string(times[1].times) + "次";
					}
				}
			}
		}
		break;
	case 3://槓
		if (move == 2)
		{
			str += "打牌、被槓";
			if (ba_times[2].continuous > 1)
			{
				str += "、被連吃" + to_string(ba_times[2].continuous) + "次";
			}
			else if (ba_times[2].times > 1)
			{
				str += "了第" + to_string(ba_times[2].times) + "次";
			}
		}
		else
		{
			str += "槓";
			if (times[2].continuous > 1)
			{
				str += "、連槓" + to_string(times[1].continuous) + "次";
			}
			else if (times[2].times > 1)
			{
				str += "了第" + to_string(times[1].times) + "次";
			}
		}
		break;
	case 4://碰槓
		str += "摸牌、碰槓";
		break;
	case 5://暗槓
		str += "摸牌、暗牌";
		break;
	case 6://進牌
		str += "摸牌、進牌";
		break;
	case 7://摸到前三輪丟出去的牌
		str += "摸牌、摸到前三輪丟過的牌";
		break;
	case 8://打危險牌
		str += "打牌、打危險牌";
		break;
	case 9://跟打
		str += "打牌、跟打";
		break;
	case 10:
		str += "摸牌、摸到無用的牌";
		break;
	case 11:
		str += "摸牌、摸到安全牌";
		break;
	case 12:
		if (duration < 15)
		{
			str += "超快聽牌";
		}
		else
		{
			str += "聽牌";
		}

		break;
	case 13:
		str += "自摸";
		break;
	case 14:
		if (move == 2)
		{
			str += "被胡牌";
		}
		else
		{
			str += "胡牌";
		}
		break;
	case 15://流局
		str += "流局";
		break;
	case 16:
		str += "連莊";
		break;
	case 17://補六張花以上
		str += "補六張花以上";
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
			str += "、送了" + to_string(count) + "牌";
		}
		else if (move == 0 && count > 3 && temp != 1)
		{
			str += "門前有" + to_string(count) + "堆";
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
	if (HGPE == COMMAND_GON)//哈拉麻將
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
	if (HGPE == COMMAND_GON)//哈拉麻將
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
		this->happen = 1;//哈拉麻將
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
	if (card.color >= 0 && card.color <= 2) {//萬條筒
		if (Table.mjcolor[card.color][card.value] >= 2) {
			is_useless = true;
		}
	}
	else if (card.color == 3) {//字牌
		if (((Table.cfbCards >> (card.value * 3)) & 7) >= 2) {
			is_useless = true;
		}
	}
	else if (card.color == 4) {//風牌
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

bool Hola::CheckLastNeed(Player player, CommandType HGPE, const MJCard & card, const MJCards & Discards)//哈拉麻將
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
		if (card.color >= 0 && card.color <= 2) {//萬條筒
			if ((Discards.mjcolor[card.color][card.value] + player.Hand.mjcolor[card.color][card.value]) == 4) {
				is_the_last = true;
			}
		}
		else if (card.color == 3) {//字牌
			if (((Discards.cfbCards >> (card.value * 3)) & 7) + (player.Hand.cfbCards >> (card.value * 3) & 7) == 4) {
				is_the_last = true;
			}
		}
		else if (card.color == 4) {//風牌
			if (((Discards.windsCards >> (card.value * 3)) & 7) + (player.Hand.windsCards >> (card.value * 3) & 7) == 4) {
				is_the_last = true;
			}
		}
	}
	else if (HGPE == NONE)//代表用摸牌的
	{

	}

	return is_the_last;
}

bool Hola::CheckIsSafe(const MJCard & card, const MJCards & Discards)
{
	bool is_safe = true;
	if (card.color >= 0 && card.color <= 2) {//萬條筒
		if (Discards.mjcolor[card.color][card.value] <= 2) {
			is_safe = false;
		}
	}
	else if (card.color == 3) {//字牌
		if (((Discards.cfbCards >> (card.value * 3)) & 7) <= 2) {
			is_safe = false;
		}
	}
	else if (card.color == 4) {//風牌
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

