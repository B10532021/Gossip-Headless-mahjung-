#pragma once
#include "Node.h"

class NormalNode : public Node {
public:
	NormalNode(const size_t player, const MJCards &hand, const MJCards &left, const MJCards &door, const size_t wallCount, const double bias = 0, Node *parent = NULL, const MJCard &actionCard = MJCard(), const int isLeaf_value = -1);
	Node *SelectNext(const int totalTimes, const int explorationParameter); // UCT
	void Expand(); // 手牌必須排序過，丟出每種不同的手牌各算一個child
	bool AllowSimulation() const { return true; };
};