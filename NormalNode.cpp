#include "ChanceNode.h"
#include "NormalNode.h"
#include "MCTS.h"

NormalNode::NormalNode(const size_t player, const MJCards &hand, const MJCards &left, const MJCards &door, const size_t wallCount, const double bias, Node *parent, const MJCard &actionCard, const int isLeaf_value) :
	Node(player, hand, left, door, wallCount, bias, parent, actionCard, isLeaf_value) {
	//std::cout << "normal " << std::endl;
}

Node *NormalNode::SelectNext(const int totalTimes, const int explorationParameter) {
	double max = -1;
	unsigned max_idx = 0;
	unsigned start = rand() % children.size(); // 磷UCTッ环匡材
	for (unsigned i = 0; i < children.size(); i++) {
		unsigned idx = (i + start) % children.size();
		double uct = children[idx]->UCT(totalTimes, explorationParameter);
		if (uct > max) {
			max = uct;
			max_idx = idx;
		}
	}
	return children[max_idx];
}

void NormalNode::Expand() {
	if (children.size() > 0)
		return;
	int color = actionCard.color, value = actionCard.value;
	if (player == 0) { //產
		if (color != -1) { // 篘礟篵篘窱ぃ
			int score = MCTS::CalculateScore(hand, door, actionCard);
			if (score != 0) { // 篘传產篘礟
				children.push_back(new NormalNode(1, MJCards(hand) -= actionCard, left, door, wallCount, 3, this, MJCard(-1, -1), score));
			}
			if (wallCount > 0) {
				hand.ForEach([this](int color, int value, int number) {
					if (number == 4) { // 穞篵传產篘礟
						MJCards newHand(hand), newDoor(door);
						newHand[color][value] -= 4;
						newDoor[color][value] += 4;
						double bias = 0.9;
						if (color < 3 && (value >= 1 && hand[color][value - 1] == 1 || value <= 7 && hand[color][value + 1] == 1))
							bias = 0;
						children.push_back(new ChanceNode(0, newHand, left, newDoor, wallCount, bias, this));
					}
					else if (number == 1 && door[color][value] == 3) { // 篵传產篘礟
						double bias = 0.9;
						if (color < 3 && (value >= 1 && hand[color][value - 1] == 1 || value <= 7 && hand[color][value + 1] == 1))
							bias = 0;
						children.push_back(new ChanceNode(0, MJCards(hand) -= {color, value}, left, MJCards(door) += {color, value}, wallCount, bias, this));
					}
				});
			}
		}
		// PASS = メ礟传產篘礟
		hand.ForEach([this](int color, int value, int number) {
			double bias = 0.8;
			if (color < 3) {
				if (value <= 7 && hand[color][value + 1] == 1) {
					bias -= 0.4;
					if (value <= 6 && hand[color][value + 2] == 1) {
						bias -= 0.4;
						if (value <= 5 && hand[color][value + 3] == 1) {
							bias += 0.8;
						}
					}
				}
				if (value >= 1 && hand[color][value - 1] == 1) {
					bias -= 0.4;
					if (value >= 2 && hand[color][value - 2] == 1) {
						bias -= 0.4;
						if (value >= 3 && hand[color][value + 3] == 1) {
							bias += 0.8;
						}
					}
				}
				bias -= 0.5 * number + 0.02 * abs(value - 5);
			}
			else {
				bias -= 0.3 * number;
			}
			children.push_back(new ChanceNode(NextPlayer(), MJCards(hand) -= MJCard(color, value), left, door, wallCount, bias, this));
		});
	}
	else { // 產
		int score = MCTS::CalculateScore(hand, door, actionCard);
		if (score != 0) { // 璊礟leaf node
			children.push_back(new NormalNode(1, MJCards(hand) -= actionCard, left, door, wallCount, 1, this, MJCard(-1, -1), score));
		}
		int actionCardCount = hand[actionCard];
		if (actionCardCount == 4 && wallCount > 0) { // 篵传產篘礟
			MJCards newHand(hand), newDoor(door);
			newHand[actionCard] -= 4;
			newDoor[actionCard] += 4;
			double bias = 0.9;
			if (color < 3) {
				if (value <= 7 && hand[color][value + 1] == 1) {
					bias -= 0.45;
					if (value <= 6 && hand[color][value + 2] == 1) {
						bias -= 0.45;
						if (value <= 5 && hand[color][value + 3] == 1) {
							bias += 0.9;
						}
					}
				}
				if (value >= 1 && hand[color][value - 1] == 1) {
					bias -= 0.45;
					if (value >= 2 && hand[color][value - 2] == 1) {
						bias -= 0.45;
						if (value >= 3 && hand[color][value + 3] == 1) {
							bias += 0.9;
						}
					}
				}
			}
			children.push_back(new ChanceNode(0, newHand, left, newDoor, wallCount, bias, this));
		}
		if (actionCardCount >= 3) { // 窱传產メ礟
			MJCards newHand(hand), newDoor(door);
			newHand[actionCard] -= 3;
			newDoor[actionCard] += 3;
			double bias = 0.9;
			if (color < 3) {
				if (actionCardCount == 3) {
					if (value <= 7 && hand[color][value + 1] == 1) {
						bias -= 0.45;
						if (value <= 6 && hand[color][value + 2] == 1) {
							bias -= 0.45;
							if (value <= 5 && hand[color][value + 3] == 1) {
								bias += 0.9;
							}
						}
					}
					if (value >= 1 && hand[color][value - 1] == 1) {
						bias -= 0.45;
						if (value >= 2 && hand[color][value - 2] == 1) {
							bias -= 0.45;
							if (value >= 3 && hand[color][value + 3] == 1) {
								bias += 0.9;
							}
						}
					}
				}
				else if ((value == 8 || hand[color][value + 1] == 0) && (value == 0 && hand[color][value - 1] == 0)) { // 虫眎4眎
					bias = 0.3;
				}
			}
			else if (actionCardCount == 4) { // 4眎
				bias = 0.2;
			}
			children.push_back(new NormalNode(0, newHand, left, newDoor, wallCount, bias, this, MJCard(-1, -1)));
		}
		if (player == 3 && color < 3) { // 
			if (value >= 1 && hand[color][value - 1] > 0) {
				if (value >= 2 && hand[color][value - 2] > 0) {
					double bias = 0.8;
					if (actionCardCount == 2 || value >= 3 && hand[color][value - 3] == 1)
						bias -= 0.6;
					children.push_back(new NormalNode(0, ((MJCards(hand) -= actionCard) -= {color, value - 1}) -= {color, value - 2}, left, ((MJCards(door) += actionCard) += {color, value - 1}) += {color, value - 2}, wallCount, bias, this, MJCard(-1, -1)));
				}
				if (value <= 7 && hand[color][value + 1] > 0) {
					double bias = 0.9;
					if (actionCardCount == 2)
						bias -= 0.7;
					if (value >= 2 && hand[color][value - 2] == 1) {
						bias -= 0.1;
						if (value >= 3 && hand[color][value - 3] == 1) {
							bias -= 0.8;
							if (value >= 4 && hand[color][value - 4] == 1) {
								bias += 0.9;
							}
						}
					}
					if (value <= 6 && hand[color][value + 2] == 1) {
						bias -= 0.1;
						if (value <= 5 && hand[color][value + 3] == 1) {
							bias -= 0.8;
							if (value <= 4 && hand[color][value + 4] == 1) {
								bias += 0.9;
							}
						}
					}
					children.push_back(new NormalNode(0, ((MJCards(hand) -= actionCard) -= {color, value - 1}) -= {color, value + 1}, left, ((MJCards(door) += actionCard) += {color, value - 1}) += {color, value + 1}, wallCount, bias, this, MJCard(-1, -1)));
				}
			}
			if (value <= 6 && hand[color][value + 1] > 0 && hand[color][value + 2] > 0) {
				double bias = 0.8;
				if (actionCardCount == 2 || value <= 5 && hand[color][value + 3] == 1)
					bias -= 0.6;
				children.push_back(new NormalNode(0, ((MJCards(hand) -= actionCard) -= {color, value + 1}) -= {color, value + 2}, left, ((MJCards(door) += actionCard) += {color, value + 1}) += {color, value + 2}, wallCount, bias, this, MJCard(-1, -1)));
			}
		}
		// PASS传產篘礟
		children.push_back(new ChanceNode(NextPlayer(), MJCards(hand) -= actionCard, left, door, wallCount, 0.5, this));
	}
}