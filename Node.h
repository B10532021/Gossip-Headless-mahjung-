#pragma once
#include <vector>
#include <iostream>
//#include <boost/thread.hpp>
#include "MJCards.h"
#include "ScoreCalc.h"

const size_t NUM_SUITS = 3; // 3�ت��
const size_t NUM_RANKS = 9; // 9�ؼƭ�

class Node {
public:
	size_t player; // 0: �ۮa, 1: �U�a, 2: ��a, 3: �W�a
	MJCards hand; // ��P
	MJCards left; // �Ѿl
	MJCards door; // ���e

	bool allPon;
	bool allChow;
	bool noBonus;

	Node *parent;
	size_t wallCount; // �P��ѴX�i

	MJCard actionCard; //�Y�B�I�᪺actionCard��MJCard(-1, -1)

	std::vector<Node *> children;
	double score; // �֭p����
	int times; // �֭p���X����
	int isLeaf_value; // �J�����Ʃάy����0�A�_�h-1
	double bias;

						 // hand: ��P, left: �Ѿl�i��, door: ���e
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
