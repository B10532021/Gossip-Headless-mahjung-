#pragma once
#include <ctime>
#include "NormalNode.h"
#pragma warning( disable : 4996 )
#include "MCTSParameters.h"
//#include <boost/thread.hpp> 
#include <thread>  
#include <shared_mutex>

class MCTS {
public:
	static int CalculateScore(const MJCards &hand, const MJCards &door, const MJCard &actionCard);

	NormalNode * const root;
	const int maxTimes;
	const double maxTimeInSec;
	const int explorationParameter;
	//const size_t lack; // 0¸U 1µ© 2 ±ø

	MCTS(const int player, const MJCards &hand, const MJCards &left, const MJCards &door, const size_t wallCount, const MJCard &actionCard,
		const int maxTimes, const double maxTimeInSec, const int explorationParameter);
	~MCTS();
	Node *MakeDecision();
	void MainOperations();
private:
	std::time_t startTime;
	// Node *currentNode;
	int totalTimes;

	void Select(Node *&currentNode);
	void Expand(Node *&currentNode);
	double Simulate(Node *const currentNode);
	void Backpropagate(const int score, Node *&currentNode);

private:
	//boost::shared_mutex mutex;
	//boost::thread_group threads;
	std::shared_mutex mutex;
	std::thread threads[THREAD_NUM];
	bool timeOver;
};