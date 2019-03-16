#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include "MCTS.h"
#include "NormalNode.h"
#include "ChanceNode.h"
#include "GameManager.h"

int MCTS::CalculateScore(const MJCards &hand, const MJCards &door, const MJCard &actionCard)
{
	HuInfo info;
	for (int i = 0; i < 3; ++i)
		info.hands[i] = hand.mjcolor[i].value;
	info.hands[3] = hand.cfbCards << 12 | hand.windsCards;
	for (int i = 0; i < 3; ++i)
		info.doors[i] = door.mjcolor[i].value;
	info.doors[3] = door.cfbCards << 12 | door.windsCards;
	switch (actionCard.color) {
	case 0:
		info.certainTile = actionCard.value;
		break;
	case 1:
		info.certainTile = actionCard.value + 9;
		break;
	case 2:
		info.certainTile = actionCard.value + 18;
		break;
	case 3:
		info.certainTile = actionCard.value + 31;
		break;
	case 4:
		info.certainTile = actionCard.value + 27;
		break;
	default:
		cout << "CCardTo34 error" << endl;
		cout << "color = " << actionCard.color << ", value = " << actionCard.value << endl;
		hand.Print();
		door.Print();
		system("pause");
		return -1;
	}
	info.allChow = info.allPon = false;
	info.isZimo = false;
	info.noBonus = false;
	info.winds.all = 0b1100;
	auto result = ScoreCalc::getInstance().countTai(info);
	//auto[tai, str] = ScoreCalc::getInstance().countTai(info);
	//cout << ScoreCalc::huInfoToString(info) << endl;
	//if(tai != -1)
	//	cout << "�x: " << tai << " " << "�P��: " << str << endl;
	return pow(2, result.first - 1);
}

MCTS::MCTS(const int player, const MJCards &hand, const MJCards &left, const MJCards &door, const size_t wallCount, const MJCard &actionCard, const int maxTimes, const double maxTimeInSec, const int explorationParameter) :
	root(new NormalNode(player, hand, left, door, wallCount, 0, NULL, actionCard)), totalTimes(0), maxTimes(maxTimes), maxTimeInSec(maxTimeInSec), timeOver(false), explorationParameter(explorationParameter) {
	std::srand((unsigned)std::time(NULL));
	startTime = std::clock();
}

MCTS::~MCTS() {
	delete root;
}

Node *MCTS::MakeDecision() {
	//for (int i = 0; i < THREAD_NUM; ++i)
	//	threads.create_thread(boost::bind(&MCTS::MainOperations, this));
	for (std::thread &t : threads)
		t = std::thread(&MCTS::MainOperations, this);

#ifdef USE_TIME_LIMIT
	std::this_thread::sleep_for(std::chrono::seconds((int)maxTimeInSec));
	//Sleep(maxTimeInSec * 1000);
	timeOver = true;
	//threads.interrupt_all();
#endif

	//threads.join_all();
	for (std::thread &t : threads)
		t.join();

	int maxTimes = -1;
	size_t maxTimeIdx = 0;
	//root->hand.Print();
	for (size_t i = 0; i < root->children.size(); ++i) {
		//std::cout << root->children[i]->times << " " << root->children[i]->score << " " << root->children[i]->UCT(totalTimes) << std::endl;
		//root->children[i]->hand.Print();
		if (root->children[i]->times > maxTimes) {
			maxTimes = root->children[i]->times;
			maxTimeIdx = i;
		}
	}
	std::cout << "MCTS: time = " << (double)(std::clock() - startTime) / CLOCKS_PER_SEC << "s" << ", times = " << maxTimes << " / " << totalTimes << std::endl << std::endl;
	return root->children[maxTimeIdx];
}

void MCTS::MainOperations()
{
	Node *currentNode = root;
	while (!timeOver) {
		Select(currentNode);
		//boost::this_thread::interruption_point();
		if (currentNode->isLeaf_value == -1) {
			Expand(currentNode);
			if (currentNode->AllowSimulation()) {
#ifdef MULTIPLE_SAMPLINGS
				int sampleCount = 0;
				double squareSum = 0, sum = 0, avg = 0;
				while (true) {
					for (int i = 0; i < SAMPLING_SCALE; ++i) {
						double v = Simulate(currentNode);
						//printf("%d//", v);
						sum += v;
						squareSum += v * v;
					}
					sampleCount += SAMPLING_SCALE;
					avg = sum / sampleCount;
					if (squareSum / sampleCount - avg * avg > SAMPLING_THRESHOLD * SAMPLING_THRESHOLD)
						break;
				}
				double reward = avg;
				//if (sampleCount!= SAMPLING_SCALE)
				//printf("%d;%f;;;", sampleCount, squareSum / sampleCount - avg * avg);
#else
				double reward = Simulate(currentNode);
#endif
				Backpropagate(reward, currentNode);
			}
		}
		else {
			Backpropagate(currentNode->isLeaf_value, currentNode);
		}		
		//boost::this_thread::interruption_point();
		//std::cout << "test" << std::endl;
		//boost::this_thread::interruption_point();

#ifndef USE_TIME_LIMIT
		if (totalTimes >= maxTimes)
			break;
#endif
	}
}

void MCTS::Select(Node *&currentNode) {
	std::shared_lock lock(mutex);
	//boost::shared_lock<boost::shared_mutex> lock(mutex);
	while (currentNode->children.size() != 0) {
		//currentNode = currentNode->SelectNext(totalTimes);
		auto tmp = currentNode->SelectNext(totalTimes, explorationParameter);
		if (!tmp) {
			printf("Error in Select: children.size() = %d, %s\n", currentNode->children.size(), currentNode->AllowSimulation() ? "NormalNode" : "ChanceNode");
			system("pause");
		}
		currentNode = tmp;
	}
}

void MCTS::Expand(Node *&currentNode) {
	//boost::lock_guard<boost::shared_mutex> lock(mutex);
	/*
	// root��<���O�즸���X�B�٨S�����B�٨S�J>
	if (currentNode == root ||
		currentNode->times != 0 &&
		currentNode->wallCount >= 1U - (currentNode->hand.Count() + currentNode->door.Count() == 14) &&
		currentNode->isLeaf_value == 0) {
	*/
	// �ۮa��P�BĲ�o�S��ʧ@�ɡGwallCount >= 0�]�b��Node::Expand()�P�w�^
	// �ۮa�N�P�ɡGwallCount >= 1
	//if (currentNode->wallCount >= 2 - currentNode->hand.Count() % 3 &&
	//	currentNode->isLeaf_value == -1) {
		//boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);	
	//}
	{
		std::lock_guard lock(mutex);
		currentNode->Expand();
	}
	if (currentNode->children.size() != 0)
		currentNode = currentNode->children[(unsigned)rand() % currentNode->children.size()]; // �H����
}

double MCTS::Simulate(Node *const currentNode) {
	/*
	Node *cur = currentNode;
	cout << "vvvvv " << cur->hand.Count() << endl;
	cout << "wwwww " << cur->door.Count() << endl;
	while (cur != root) {
		cur = cur->parent;
		cout << "vvvvv " << cur->hand.Count() << endl;
		cout << "wwwww " << cur->door.Count() << endl;
	}
	*/
	MJCards hand, left, door;
	size_t wallCount;
	int firstStartIdx;
	MJCard actionCard;
	{
		std::shared_lock lock(mutex);
		//boost::shared_lock<boost::shared_mutex> lock(mutex);

		if (currentNode->isLeaf_value != -1) // �w�g���������ݭn����
			return currentNode->isLeaf_value;

		hand = currentNode->hand;
		left = currentNode->left;
		door = currentNode->door;
		wallCount = currentNode->wallCount;
		firstStartIdx = currentNode->player;
		actionCard = currentNode->actionCard;
	}

	if (firstStartIdx == 0) {
		if (hand.Count() % 3 == 2) {
			if (actionCard.color == -1)
				goto STATE1;
			goto STATE0;
		}
		goto STATE3;
	}
	goto STATE2;

	while (true) { // �P���٨S����
		//boost::this_thread::interruption_point();
	STATE0: // �ۺN�B�t�b�B�[�b
//std::cout << "000000000000";////////////////////////////////////////////
		if (root->actionCard.color != -1) {
			//if (actionCard.color == -1) std::cout << "aaaaaaaaaaaaaaaaaa";////////////////////////////////////////////
			int score = MCTS::CalculateScore(hand, door, actionCard);
			if (score != 0)
				return score; // �ۺN
			if (wallCount > 0) {
				if (hand.Find([&](int color, int value, int number) {
					if (number == 4 && rand() % 2 == 0) { // ��t�b�G���ۮa�N�P
						hand[color][value] -= 4;
						door[color][value] += 4;
						return true;
					}
					else if (number == 1 && door[color][value] == 3 && rand() % 2 == 0) { // ��[�b�G���ۮa�N�P
						--hand[color][value];
						--door[color][value];
						return true;
					}
				}).color != -1)
					goto STATE3;
			}
		}
	STATE1: // �ۮa��@�i�P
		{
//std::cout << "1111111111111";////////////////////////////////////////////
			size_t i, j;
			// ���i�����P
			if (hand.windsCards > 0) {
				i = 4;
				for (j = 0; j < 4; ++j)
					if (hand[4][j] == 1)
						goto SIM_THROW_END;
			}
			// ���i���r�P
			if (hand.cfbCards > 0) {
				i = 3;
				for (j = 0; j < 3; ++j)
					if (hand[3][j] == 1)
						goto SIM_THROW_END;
			}
			{
				size_t suitSorted[3] = { 0, 1, 2 }; // ����ƶq�֪����
				static const size_t rankSorted[NUM_RANKS] = { 0, 8, 1, 7, 2, 6, 3, 5, 4 };
				std::sort(suitSorted, suitSorted + 3, [&hand, &door](size_t a, size_t b) {
					return hand.mjcolor[a].Count() + door.mjcolor[a].Count() < hand.mjcolor[b].Count() + door.mjcolor[b].Count();
				});
				// ���i���U����
				for (int _i = 0; _i < NUM_SUITS; ++_i) {
					i = suitSorted[_i];
					for (int _j = 0; _j < NUM_RANKS; ++_j) {
						j = rankSorted[_j];
						if (hand.mjcolor[i][j] == 1 && (j == 0 || hand.mjcolor[i][j - 1] == 0) && (j == 8 || hand.mjcolor[i][j + 1] == 0))
							goto SIM_THROW_END;
					}
				}
				// ��Ƕ��l���U����
				for (int _i = 0; _i < NUM_SUITS; ++_i) {
					i = suitSorted[_i];
					for (int _j = 0; _j < NUM_RANKS; ++_j) {
						j = rankSorted[_j];
						if (hand.mjcolor[i][j] == 1 && (j <= 1 || hand.mjcolor[i][j - 1] == 0 || hand.mjcolor[i][j - 2] == 0) && (j >= 7 || hand.mjcolor[i][j + 1] == 0 || hand.mjcolor[i][j + 2] == 0) && (j == 0 || j == 8 || hand.mjcolor[i][j - 1] == 0 || hand.mjcolor[i][j + 1] == 0))
							goto SIM_THROW_END;
					}
				}
				// �ᦨ�諸�L�f���P
				if (hand.windsCards > 0) {
					i = 4;
					for (j = 0; j < 4; ++j)
						if (hand[4][j] == 2)
							goto SIM_THROW_END;
				}
				// �ᦨ�諸�r�P
				if (hand.cfbCards > 0) {
					i = 3;
					for (j = 0; j < 3; ++j)
						if (hand[3][j] == 2)
							goto SIM_THROW_END;
				}
				// �ᦨ�諸�U����
				for (int _i = 0; _i < NUM_SUITS; ++_i) {
					i = suitSorted[_i];
					for (int _j = 0; _j < NUM_RANKS; ++_j) {
						j = rankSorted[_j];
						if (hand.mjcolor[i][j] == 2 && (j == 0 || hand.mjcolor[i][j - 1] == 0) && (j == 8 || hand.mjcolor[i][j + 1] == 0))
							goto SIM_THROW_END;
					}
				}
				// �H�K��
				hand -= hand.at(0);
//if (hand.Count() % 3 == 2) std::cout << "_7777777777777";////////////////////////////////////////////
				goto STATE2;
			}
		SIM_THROW_END:
			--hand[i][j];
//if (hand.Count() % 3 == 2) std::cout << "7777777777777";////////////////////////////////////////////
		}
	STATE2: // �L�a�U��@�i�P
		{
//if (hand.Count() % 3 == 2 && actionCard.color == -1) std::cout << "222222222222";////////////////////////////////////////////
			const size_t oppenentNum = firstStartIdx == 0 ? 3 : (4 - firstStartIdx) % 4;
			firstStartIdx = 0;
			for (size_t i = 0; i < oppenentNum; ++i) {
				// copy = hand + actionCard
				MJCards copy(hand);
				if (hand.Count() % 3 == 1) { // �L�a�N�P
//if (actionCard.color == -1) std::cout << "cccccccccccccc" << std::endl;////////////////////////////////////////////
					while (true) { // ��P�j��
						if (wallCount == 0) // �P������
							return 0;
						actionCard = left.Rand();
						//std::cout << "dddddddddddddddd" << std::endl;////////////////////////////////////////////
						left -= actionCard;
						--wallCount;
						if (actionCard.color != 5) { // ���O��
							copy += actionCard;
							break;
						}
					}
				}
				else { // �L�a�w��PĲ�o�ƥ�
//std::cout << "eeeeeeeeeeeeee" << std::endl;////////////////////////////////////////////
					--hand[actionCard];
				}
//if (actionCard.color == -1) std::cout << "bbbbbbbbbbbbbbbbbb";////////////////////////////////////////////
				int score = MCTS::CalculateScore(copy, door, actionCard);
				if (score >= 2 || rand() % 2 == 0) {
					return score; // �J�F
				}
				bool forcePon = false;
				if (copy[actionCard] == 4 && wallCount > 0) { // ����b�G���ۮa�N�P
					if (rand() % 2 == 0) {
						hand[actionCard] -= 3;
						door[actionCard] += 4;
						goto STATE3;
					}
					else {
						forcePon = true;
					}
				}
				if (forcePon || copy[actionCard] >= 3 && rand() % 2 == 0) { // ��I�G���ۮa��P
					hand[actionCard] -= 2;
					door[actionCard] += 3;
					goto STATE1;
				}
				if (i == oppenentNum - 1 && actionCard.color < 3 && rand() % 2 == 0) { // ��Y�G���ۮa��P
					if (actionCard.value >= 1) {
						MJCard left1(actionCard.color, actionCard.value - 1);
						if (hand.Have(left1)) {
							if (actionCard.value >= 2) {
								MJCard left2(actionCard.color, actionCard.value - 2);
								if (hand.Have(left2)) {
									(hand -= left1) -= left2;
									((door += actionCard) += left1) += left2;
									goto STATE1;
								}
							}
							else if (actionCard.value <= 7) {
								MJCard right1(actionCard.color, actionCard.value + 1);
								if (hand.Have(right1)) {
									(hand -= left1) -= right1;
									((door += actionCard) += left1) += right1;
									goto STATE1;
								}
							}
						}
					}
					else if (actionCard.value <= 6) {
						MJCard right1(actionCard.color, actionCard.value + 1);
						MJCard right2(actionCard.color, actionCard.value + 2);
						if (hand.Have(right1) && hand.Have(right2)) {
							(hand -= right1) -= right2;
							((door += actionCard) += right1) += right2;
							goto STATE1;
						}
					}
				}
			}
		}
	STATE3: // �ۮa�N�@�i�P
		{
//std::cout << "3333333333333";////////////////////////////////////////////
			if (wallCount == 0) // �P������
				return 0;
			actionCard = left.Rand();
			left -= actionCard;
			--wallCount;
			if (actionCard.color == 5) { // ��P
				door += actionCard;
				goto STATE3;
			}
			hand += actionCard;
		}
	}
	return 0;
}

void MCTS::Backpropagate(const int score, Node *&currentNode) {
	std::lock_guard lock(mutex);
	//boost::lock_guard<boost::shared_mutex> lock(mutex);

	//std::cout << score << std::endl;
	++totalTimes;
	do { // todo: ���chance node���Χ�s
		currentNode->score += score;
		++currentNode->times;
	} while (currentNode = currentNode->parent);
	currentNode = root;
}