#pragma once
#include "Node.h"

class ChanceNode : public Node {
public:
	ChanceNode(const size_t player, const MJCards &hand, const MJCards &left, const MJCards &door, const size_t wallCount, const double bias = 0, Node *parent = NULL, const int isLeaf_value = -1);
	Node *SelectNext(const int totalTimes, const int explorationParameter); // ChanceNode���|�Ψ�totalTimes
	void Expand(); // �ۮa�N��/�L�a���X�C�ؤ��P����P�U��@��child
				   // �̾��v�����^�Ǥ@��childIdx
	bool AllowSimulation() const { return false; };
	static size_t RandomSelect(const MJCards &left);
};