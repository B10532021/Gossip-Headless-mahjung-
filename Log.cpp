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
	const string winds[4] = { "�F", "�n", "��", "�_" };
	fout << winds[round] << "��" << winds[door] << ",";
	fout<< "��," << players[dealer]->uid << " ,�s," << gameManager->numKeepWin << endl;

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
		token = ",�N,";
		break;
	case DISCARD:
		token = ",��,";
		break;
	case REPLACE:
		token = ",�ɪ�,";
		break;
	case CHOW:
		token = ",�Y,";
		break;
	case PONG:
		token = ",�I,";
		break;
	case KONG:
		token = ",�b,";
		break;
	case ADD_KONG:
		token = ",�[�b,";
		break;
	case CONCEALED_KONG:
		token = ",�t�b,";
		break;
	case HU:
		token = ",�J,";
		break;
	case ZIMO:
		token = ",�ۺN,";
		break;
	}

	fout << "," << actionPlayer->uid << token <<
		tile.value << C2W[tile.color] <<
		actionPlayer->Print() << std::endl;
}


void GameManager::CsvLog::logResult(const short& tai, const short& total_tai, const std::string& str)
{
	fout << "�x," << tai << ",�`�x," << total_tai << ",�P��," << str << std::endl;

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