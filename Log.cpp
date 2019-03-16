#include "GameManager.h"


GameManager::Log::Log(GameManager* gameManager, const std::string& fileName)
{
	this->gameManager = gameManager;
	//fout.open(fileName, std::ios::binary);
}



//----------------CsvLog---------------------
GameManager::CsvLog::CsvLog(GameManager* gameManager, const std::string& fileName)
{
	this->gameManager = gameManager;
	fout.open(fileName);
}

void GameManager::CsvLog::logStart(const int& round, const int& door)
{
	const auto& dealer = gameManager->dronIdx;
	const string winds[4] = { "東", "南", "西", "北" };
	fout << winds[round] << "風" << winds[door] << ",";
	fout<< "莊," << players[dealer]->uid << " ,連," << gameManager->numKeepWin << endl;

	for (int i = 0; i < 4; i++) {
		const auto& player = players[i];
		fout << "," << player->uid << ",," << player->Print() << endl;
	}
}

void GameManager::CsvLog::logState(const State& action, const MJCard& tile, const int& s, const int& t) 
{
	const auto& actionPlayer = players[s];
	std::string token;
	switch (action) {
	case DRAW:
		token = ",摸,";
		break;
	case DISCARD:
		token = ",丟,";
		break;
	case REPLACE:
		token = ",補花,";
		break;
	case CHOW:
		token = ",吃,";
		break;
	case PONG:
		token = ",碰,";
		break;
	case KONG:
		token = ",槓,";
		break;
	case ADD_KONG:
		token = ",加槓,";
		break;
	case CONCEALED_KONG:
		token = ",暗槓,";
		break;
	case HU:
		token = ",胡,";
		break;
	case ZIMO:
		token = ",自摸,";
		break;
	}

	fout << "," << actionPlayer->uid << token <<
		tile.value << C2W[tile.color] <<
		actionPlayer->Print() << std::endl;
}


void GameManager::CsvLog::logResult(const short& tai, const short& total_tai, const std::string& str)
{
	fout << "台," << tai << ",總台," << total_tai << ",牌型," << str << std::endl;

	fout << "score,"
		 << gameManager->score[0] << ","
		 << gameManager->score[1] << ","
		 << gameManager->score[2] << ","
		 << gameManager->score[3] << std::endl;
}

void GameManager::CsvLog::writeLog(const std::string& str)
{
	fout << str << std::endl;
}

void GameManager::CsvLog::logConversation(const std::string & str)
{
	if (str != "")
	{
		fout << str << std::endl;
	}
}

GameManager::CsvLog::~CsvLog()
{
	fout.close();
}