#pragma once
#include <vector>
#include <iostream>
//#include <boost/thread.hpp>
#include "MJCards.h"
#include "ScoreCalc.h"

const size_t NUM_SUITS = 3; // 3種花色
const size_t NUM_RANKS = 9; // 9種數值

class Node {
public:
	size_t player; // 0: 自家, 1: 下家, 2: 對家, 3: 上家
	MJCards hand; // 手牌
	MJCards left; // 剩餘
	MJCards door; // 門前

	bool allPon;
	bool allChow;
	bool noBonus;

	Node *parent;
	size_t wallCount; // 牌堆剩幾張

	MJCard actionCard; //吃、碰後的actionCard為MJCard(-1, -1)

	std::vector<Node *> children;
	double score; // 累計分數
	int times; // 累計拜訪次數
	int isLeaf_value; // 胡的分數或流局的0，否則-1
	double bias;

						 // hand: 手牌, left: 剩餘張數, door: 門前
	Node(const size_t player, const MJCards &hand, const MJCards &left, const MJCards &door, const size_t wallCount, const double bias = 0, Node *parent = NULL, const MJCard &actionCard = MJCard(-1, -1), const int isLeaf_value = -1);
	~Node() {
		for (Node *child : children)
			delete child;
	}

	size_t NextPlayer() {
		return (player + 1) % 4;
	}

	// override method
	virtual Node *SelectNext(const int totalTimes, const int explorationParameter) { return 0; };
	virtual void Expand() {};

	virtual bool AllowSimulation() const { return true; };

	double UCT(const int totalTimes, const int explorationParameter);
};
