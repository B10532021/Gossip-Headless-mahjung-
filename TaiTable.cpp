#include "TaiTable.h"

SimplesTaiTable::SimplesTaiTable()
{
	initTable();
	//test();
}

std::pair<short, std::string> SimplesTaiTable::operator[](Feature idx) const
{
	auto it = taiTable.find(idx);
	if (it == taiTable.end())
		return std::make_pair(-1, "");
	else
		return it->second;
}


void SimplesTaiTable::initTable()
{
	for (unsigned int i = 0; i < 0x1fff; i++) {
		if ((i & CAN_HU) && (i & HAS_EYE)) {
			short tai = 0;
			std::string str;
			// 自摸
			if (i & ZIMO) {
				tai += 1;
				str += "自摸 ";
			}
			// 全求人(自摸不計) or 門清
			if (i & ONLY_HAS_EYE && !(i & ZIMO)) {
				tai += 2;
				str += "全求人 ";
			}
			else if (i & CLEAN) {
				tai += 1;
				str += "門清 ";
				//門清自摸加一台
				tai += (i & ZIMO) ? 1 : 0;
			}
			if ((i & ONLY_HAS_EYE) && (i & CLEAN))
				continue;

			// 獨聽(全求人不計)
			if (i & TIN_ONLY_ONE && !(i & ONLY_HAS_EYE && !(i & ZIMO))) {
				tai += 1;
				str += "獨聽 ";
			}

			// 湊一色 or 清一色
			if (i & MIX_SUIT) {
				tai += 4;
				str += "湊一色 ";
			}
			else if (i & SAME_SUIT) {
				tai += 8;
				str += "清一色 ";
			}
			else if((i& MIX_SUIT) && (i & SAME_SUIT))
				continue;
			// 碰碰胡
			if (i & PON_PAIR) {
				tai += 4;
				str += "碰碰胡 ";
			}

			// 三暗刻 四暗刻 五暗刻
			auto cnt = (i >> 9) & 0b111;
			if (cnt > 6)
				continue;
			if (!(i & ONLY_HAS_EYE)) {
				//五暗刻必門清和碰碰胡
				if ((i & CLEAN) && (i & PON_PAIR)) {
					if (cnt == 5) {
						tai += 8;
						str += "五暗刻 ";
					}
					else if (cnt == 4) {
						tai += 5;
						str += "四暗刻 ";
					}
				}
				else {
					if (cnt == 4) {
						tai += 5;
						str += "四暗刻 ";
					}
					else if (cnt == 3) {
						tai += 2;
						str += "三暗刻 ";
					}
				}
			}
			
			taiTable[i] = std::make_pair(tai, str);
		}
	}
	// 平胡
	taiTable[PING_HU | HAS_EYE | CAN_HU] = std::make_pair(2, "平胡 ");
	// 門清+平胡
	taiTable[CLEAN | PING_HU | HAS_EYE | CAN_HU] = std::make_pair(3, "門清 平胡 ");
}


void SimplesTaiTable::test()
{
	int i = 0;
	for (const auto &t : taiTable) {
		//std::cout << std::bitset<16>(t.first) << " " << 
			//t.second.first << " " << t.second.second << std::endl;
		i++;
	}
	// 2047
	std::cout << "\nTotal: " << i << std::endl;
	std::cout << "----------------------------\n";
}

std::ostream& operator<<(std::ostream& out, const SimplesTaiTable& obj)
{
	for (const auto& [key, value] : obj.taiTable)
		out << key << " " << value.first << " " << value.second << std::endl;
	return out;
}




//-----------------------------
//HonorTaiTable
HonorsTaiTable::HonorsTaiTable()
{
	initTable();
}

std::pair<short, std::string> HonorsTaiTable::operator[](Feature idx) const
{
	auto it = taiTable.find(idx);
	if (it == taiTable.end())
		return std::make_pair(-1, "");
	else
		return it->second;
}

void HonorsTaiTable::initTable()
{
	for (Feature i = 0; i < (1 << 12); i++) {
		int tai = 0;
		std::string str;
		if (i & LITTLE_FOUR_WINDS) {
			tai += 8;
			str += "小四喜 ";
		}
		else if (i & BIG_FOUR_WINDS) {
			tai += 16;
			str += "大四喜 ";
		}
		else if (i & LITTE_THREE_DRAGONS) {
			tai += 4;
			str += "小三元 ";
		}
		else if (i& BIG_THREE_DRAGONS) {
			tai += 8;
			str += "大三元 ";
		}
		int honorCnt = 0;
		const int honorsTai = (i & BONUS) ? 2 : 1;

		if (i & EAST) {
			tai += honorsTai;
			str += (i & BONUS) ? "東風東 " : "東風 ";
			honorCnt++;
		}
		if (i & SOUTH) {
			tai += honorsTai;
			str += (i & BONUS) ? "南風南 " : "南風 ";
			honorCnt++;
		}
		if (i & WEST) {
			tai += honorsTai;
			str += (i & BONUS) ? "西風西 " : "西風 ";
			honorCnt++;
		}
		if (i & NORTH) {
			tai += honorsTai;
			str += (i & BONUS) ? "北風北 " : "北風 ";
			honorCnt++;
		}
		if (i & RED) {
			tai += 1;
			str += "紅中 ";
			honorCnt++;
		}
		if (i & GREEN) {
			tai += 1;
			str += "青發 ";
			honorCnt++;
		}
		if (i & WHITE) {
			tai += 1;
			str += "白板 ";
			honorCnt++;
		}
		if (honorCnt > 6)
			continue;
		taiTable[i] = std::make_pair(tai, str);
	}
}



//-----------------------------
//TaiTable
std::pair<short, std::string> TaiTable::operator[](FeatureAll idx) const
{
	const auto[sTai, sStr] = sTable[idx & 0xffff];
	const auto[hTai, hStr] = hTable[idx >> 20];
	auto tai = (sTai + hTai == -2) ? -1 : sTai + hTai;

	return std::make_pair(tai, sStr + hStr);
}