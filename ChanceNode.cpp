#include <vector>
#include <cstdlib>
#include <cmath>
#include "ChanceNode.h"
#include "NormalNode.h"
#include "MJCards.h"
//#include "Select.h"

ChanceNode::ChanceNode(const size_t player, const MJCards &hand, const MJCards &left, const MJCards &door, const size_t wallCount, const double bias, Node *parent, const int isLeaf_value) :
	Node(player, hand, left, door, wallCount, bias, parent, MJCard(-1, -1), wallCount == 0 && isLeaf_value == -1 ? 0 : isLeaf_value) {
	//std::cout << "chance " << std::endl;
}

Node *ChanceNode::SelectNext(const int totalTimes, const int explorationParameter) {
	size_t tmp = RandomSelect(left);
	if (tmp >= children.size()) {
		printf("Error in SelectNext: RandomSelect(left) = %d, children.size() = %d\n", tmp, children.size());
		system("pause");
	}
	return children[tmp];
}

// �ȥ���call srand()
// �̾��v�����^�Ǥ@��childIdx
size_t ChanceNode::RandomSelect(const MJCards &left) {
	int childIdx = -1;
	//assert(len != 0);
	int randIdx = rand() % left.Count();
	left.Find([&childIdx, randIdx](int color, int value, int number) mutable {
		++childIdx;
		return (randIdx -= number) < 0;
	});
	//for (size_t i = 0; (i += left.Count(left.at(i))) - 1 < randIdx; ++childIdx);
	return childIdx;
}

void ChanceNode::Expand() {
	if (children.size() > 0)
		return;
	if (wallCount == 0) // leaf
		return;
	left.ForEach([this](int color, int value, int number) {
		// �ۮa�N�P
		// �L�a���P�G�ۮa�T�{�ʧ@��PASS�]NormalMode�^
		MJCard card(color, value);
		if (color == 5) // ��P
			children.push_back(new ChanceNode(player, hand, MJCards(left) -= card, player == 0 ? MJCards(door) += card : door, wallCount - 1, 0, this));
		else
			children.push_back(new NormalNode(player, MJCards(hand) += card, MJCards(left) -= card, door, wallCount - 1, 0, this, card));
	});
}