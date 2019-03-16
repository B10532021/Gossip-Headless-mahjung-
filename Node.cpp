#include <algorithm>
#include <cmath>
#include <cfloat>
#include "Node.h"
#include "MCTSParameters.h"

Node::Node(const size_t player, const MJCards &hand, const MJCards &left, const MJCards &door, const size_t wallCount, const double bias, Node *parent, const MJCard &actionCard, const int isLeaf_value) :
	player(player), hand(hand), left(left), door(door), wallCount(wallCount), bias(bias), parent(parent), score(0), times(0), actionCard(actionCard), isLeaf_value(isLeaf_value) {
}

double Node::UCT(const int totalTimes, const int explorationParameter) {
	if (times == 0)
		return DBL_MAX;
	return (score + bias) / times + explorationParameter * sqrt(log((double)totalTimes) / times);
}