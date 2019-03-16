#pragma once
#include "Node.h"

class ChanceNode : public Node {
public:
	ChanceNode(const size_t player, const MJCards &hand, const MJCards &left, const MJCards &door, const size_t wallCount, const double bias = 0, Node *parent = NULL, const int isLeaf_value = -1);
	Node *SelectNext(const int totalTimes, const int explorationParameter); // ChanceNode不會用到totalTimes
	void Expand(); // 自家摸到/他家打出每種不同的手牌各算一個child
				   // 依機率分布回傳一個childIdx
	bool AllowSimulation() const { return false; };
	static size_t RandomSelect(const MJCards &left);
};