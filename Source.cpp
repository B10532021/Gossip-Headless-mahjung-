#include<iostream>
#include<time.h>
#include"GameManager.h"
#include<fstream>

GameManager* GameManager::gameManager = new GameManager();
using namespace std;

int main(int argc, char* argv[]) {
	srand(time(NULL));
	float start = clock();

	GameManager::gameManager->Run(3, "test2.csv");

	std::cout << (clock() - start) / 1000 << "¬í" << std::endl;
	system("pause");
	return 0;
}