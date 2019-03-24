#include"GameManager.h"
#include<iostream>
#include"Player.h"
#include"Tiles.h"
#include "MCTSPlayer.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>

#define BASE 3
#define TAI 1

Player * GameManager::players[4] = { nullptr, nullptr, nullptr, nullptr };
MJCards GameManager::Sea = MJCards(false);
map<string, vector<string>> conditions;//hola
int Hola::duration = 0;//hola
auto &calc = ScoreCalc::getInstance();
HuInfo info;

static const char* C2W = "ABCDEF";

GameManager::GameManager() {
	log = std::make_unique<CsvLog>(this, "test2.csv");
	srand(time(NULL));
	
	players[0] = new Irb00();
	players[1] = new Irb01();
	players[2] = new PlayerTest();
	players[3] = new PlayerTest();
	features[0] = Hola();
	features[1] = Hola();
	features[2] = Hola();
	features[3] = Hola();
	players[0]->uid = 0;
	players[1]->uid = 1;
	players[2]->uid = 2;
	players[3]->uid = 3;
	features[0].uid = 0;
	features[1].uid = 1;
	features[2].uid = 2;
	features[3].uid = 3;
	numKeepWin = 0;
	keepWin = false;
}

void GameManager::Run(int rounds, std::string fileName) {
	ifstream fin;//讀取句子的檔案
	string sentence;
	fin.open("sentences.csv");
	if (!fin)
	{
		return;
	}
	int count = 0;
	while (getline(fin, sentence))
	{
		istringstream istream(sentence);
		string temp[2];
		int i = 0;
		while (getline(istream, sentence, ','))
		{
			temp[i] = sentence;
			i++;
		}
		conditions[temp[0]].push_back(temp[1]);
	}
	fin.close();
	/**
	int totalRounds = rounds;
	int totalCredit[4] = { 0, 0, 0, 0 };
	cout << "Game Start!\n";
	MJCard Drew(-1, -1);
	MJCard Threw(-1, -1);*/	
	   
	//---------門前資訊
	info.isZimo = false; // 是否自己摸進
	info.allChow = false; // 門前是否全為吃牌
	info.allPon = false; // 門前是否全為碰槓牌
	info.noBonus = true; // 是否無花牌

	// 門風圈風 bit4:代表門風和圈風一樣 bit:3-0(北西南東)
	info.winds.all = 0b10010; //南風南

	play_round = 0;
	while (rounds-- > 0)
	{
		SetSeat(); //決定座位和誰先當莊
		for (int j = 0; j < 1; j++) {
			for (int i = 0; i < 1; i++) //四人輪流當莊
			{
				dronIdx = i;
				//設定info.winds.all
				info.winds.all = 0;
				if (i == j)
					info.winds.all |= 16;
				int temp = 1;
				temp = temp << j;
				info.winds.all |= temp;
				temp = 1;
				temp = temp << i;
				info.winds.all |= temp;
				winds = info.winds.all;				

				do //檢查有沒有下莊
				{
					SetTable(); //全新牌墩生成、發牌、gameOver, moPie, orderChange, 玩家手牌初始化
					BuHua(); //補花直到補完
					currentIdx = i; //i為莊家

					log->logStart(play_round, j, i);
					if (features[dronIdx].keepwin)//連莊
					{
						features[dronIdx].happen = 16;
						log->logConversation(turn, features[dronIdx].Print2());
						log->logConversation(turn, features[dronIdx].Print(conditions));
						features[dronIdx].happen = 0;
					}
					
					while (gameOver != true) //直到有人胡牌或流局
					{
						//gamelog.NewTurn(turn); 
						if (moPie) //有沒有要摸牌
						{
							DrawCard(); //摸牌，直到槓及補花結束 
							if (gameOver) {
								features[dronIdx].CheckKeepWin(keepWin);
								break;
							}
						}

						AnyActions(action.second); //看其他人有沒有要湖槓碰吃, action.second 為player丟出的牌
						if (gameOver) {
							features[dronIdx].CheckKeepWin(keepWin);
							break;
						}
							
						//檢查流局
						if (Sea.Count() <= 16) {
							log->writeLog("流局");
							gameOver = true;
							keepWin = true;
							numKeepWin++;
							tie = true;
							features[dronIdx].CheckKeepWin(keepWin);
							features[dronIdx].happen = 15;
							log->logConversation(turn, features[dronIdx].Print2());
							log->logConversation(turn, features[dronIdx].Print(conditions));
							
						}
						
						
					}

				} while (keepWin);
			}
		}
	}
}

void GameManager::SetSeat()
{
	for (int i = 0; i < 4; i++) {
		int newIdx = i + rand() % (4 - i);
		std::swap(players[i], players[newIdx]);
		std::swap(features[i], features[newIdx]);
	}

	for (int i = 0; i < 4; i++) {
		uid[i] = players[i]->uid;
		players[i]->id = i;
	}
}

void GameManager::SetTable()
{
	Sea = MJCards(true); //牌墩生成
	Discards = MJCards();
	turn = 1;
	Hola::duration = 0;
	sevenHuas = false;
	sevenId = -1;
	throwRecord.clear();
	play_round++;
	for (int i = 0; i < 4; i++) {
		players[i]->Hand = MJCards();
		players[i]->eat = 0;
		players[i]->eatOrder[0] = 0;
		players[i]->eatOrder[1] = 0;
		players[i]->eatOrder[2] = 0;
		players[i]->pon = 0;
		players[i]->gon = 0;
		players[i]->anGon = 0;
		players[i]->flowers = 0;
		players[i]->winds = info.winds.all;
		players[i]->discards = &Discards;
		players[i]->throwTimes = 0;
		features[i].all_clear();
	}

	int len = 144;
	for (int i = 0; i < 4; i++) {

		for (int j = 0; j < 16; j++) {
			int idx = rand() % len;
			MJCard result = Sea.at(idx);
			Sea -= result;
			players[i]->Hand += result;
			len -= 1;
		}
	}

	gameOver = false;
	moPie = true;
}

void GameManager::BuHua()
{
	int len = 80; //海底減去四個人的牌

	//以下是補完一個人的花，再補下一個

	
	for (int i = 0; i < 4; i++)
	{
		while (players[i]->Hand.CountFlowers() != 0)
		{
			int need = players[i]->Hand.CountFlowers(); //需要補的牌數
			players[i]->flowers |= players[i]->Hand.flowersCards;
			Discards.flowersCards |= players[i]->Hand.flowersCards;
			players[i]->Hand.flowersCards = 0;
			for (int j = 0; j < need; j++)
			{
				int idx = rand() % len;
				MJCard result = Sea.at(idx);
				Sea -= result;
				players[i]->Hand += result;
				len -= 1;
			}
		}
		
	}
}

void GameManager::DrawCard()
{
	while (Sea.Count() > 16)
	{
		features[currentIdx].move = 1;//hola
		MJCard cardDrew = Sea.Draw();
		if (find(features[currentIdx].throw_before.begin(), features[currentIdx].throw_before.end(), cardDrew) != features[currentIdx].throw_before.end())
		{
			features[currentIdx].happen = 7;//拿到丟過的牌
		}
		log->logState(turn, Log::DRAW, cardDrew, currentIdx);
		//log->logConversation(features[currentIdx].Print(conditions), features[currentIdx].uid);
		//是花嗎?
		if (cardDrew.color == 5) {
			int temp = 1;
			temp = temp << cardDrew.value;
			players[currentIdx]->flowers |= temp;
			Discards.flowersCards |= temp;
			log->logState(turn, Log::REPLACE, cardDrew, currentIdx);
			if (FlowersCount(players[currentIdx]->flowers) >= 6 && Discards.CountFlowers() < 8)
			{
				features[currentIdx].happen = 17;
				log->logConversation(turn, features[currentIdx].Print2());
				log->logConversation(turn, features[currentIdx].Print(conditions));
				features[currentIdx].clear();
			}
			action.second = cardDrew;
			//Hola::duration++;
			bool hua = Check7Huas(currentIdx);
			if (hua) {
				return;
			}
			continue;
		}

		//可以胡或槓嗎
		canHu = CheckHu(currentIdx, cardDrew);		
		canGon = CheckGon(currentIdx, cardDrew, true);

		Tiles_ Hand_ = features[currentIdx].HandToTiles(players[currentIdx]->Hand);//哈拉麻將
		int pre_steptohu = features[currentIdx].StepToHu1617(Hand_);

		action = players[currentIdx]->WannaHuGon(canHu, canGon.first, cardDrew, canGon.second); 

		if (action.first == COMMAND_GON) //這邊的槓只是一個代表他要槓，不是明槓的意思
		{
			features[currentIdx].times[2].times++;//哈拉麻將
			features[currentIdx].times[2].continuous++;//哈拉麻將
			//槓摸進來那張
			if (action.second == cardDrew) {
				if (players[currentIdx]->Hand.Count(action.second) == 3) {
					for (int i = 0; i < 3; i++)
						players[currentIdx]->Hand -= cardDrew;

					players[currentIdx]->anGon |= CardTo34(cardDrew);
					features[currentIdx].happen = 5;//哈拉麻將
					log->logState(turn, Log::CONCEALED_KONG, cardDrew, currentIdx);

				}
				else if (players[currentIdx]->pon & CardTo34(cardDrew)) {
					players[currentIdx]->pon -= CardTo34(cardDrew);
					players[currentIdx]->gon |= CardTo34(cardDrew);
					features[currentIdx].happen = 4;//哈拉麻將
					log->logState(turn, Log::ADD_KONG, cardDrew, currentIdx);

					bool temp = CheckGonHu(cardDrew, currentIdx);
					if (temp) {
						return;
					}
					Discards += cardDrew;
				}
				else {
					cout << "Error, Not Enough Cards to Gon 1" << endl;
				}
			}
			else {
				if (players[currentIdx]->Hand.Count(action.second) == 4) {
					for (int i = 0; i < 4; i++)
						players[currentIdx]->Hand -= action.second;

					players[currentIdx]->anGon |= CardTo34(action.second);
					players[currentIdx]->Hand += cardDrew;
					features[currentIdx].happen = 5;//哈拉麻將
					log->logState(turn, Log::CONCEALED_KONG, action.second, currentIdx);
				}
				else if ((players[currentIdx]->pon & CardTo34(action.second)) && players[currentIdx]->Hand.Count(action.second) == 1) {
					players[currentIdx]->pon -= CardTo34(action.second);
					players[currentIdx]->gon |= CardTo34(action.second);
					players[currentIdx]->Hand += cardDrew;
					players[currentIdx]->Hand -= action.second;
					features[currentIdx].happen = 4;//哈拉麻將
					log->logState(turn, Log::ADD_KONG, action.second, currentIdx);

					bool temp = CheckGonHu(action.second, currentIdx);
					if (temp) {
						break;
					}
					Discards += action.second;
				}
				else {
					cout << "Error, Not Enough Cards to Gon 2" << endl;
				}
			}
			log->logConversation(turn, features[currentIdx].Print2());
			log->logConversation(turn, features[currentIdx].Print(conditions));
			features[currentIdx].clear();
			//Hola::duration++;
			continue;
		}
		//自摸
		else if (action.first == COMMAND_ZIMO) {
			gameOver = true;
			features[currentIdx].happen = 13;//哈拉麻將
			bool lastCard = (Sea.Count() == 16) ? true : false;
			
			PrintTai(currentIdx, cardDrew, true, winds, numKeepWin, -1, false, lastCard);
			
			if (dronIdx != currentIdx) {
				keepWin = false;
				numKeepWin = 0;
			}
			else {
				keepWin = true;
				numKeepWin++;
			}
			break;
		}
		else if (action.first == NONE) {			
			players[currentIdx]->Hand += cardDrew;

			Hand_ = features[currentIdx].HandToTiles(players[currentIdx]->Hand);//哈拉麻將
			int steptohu = features[currentIdx].StepToHu1617(Hand_);
			if (steptohu < pre_steptohu)
			{
				features[currentIdx].happen = 6;//摸牌進牌
			}
			if (features[currentIdx].other_tin || turn > 28)
			{
				if (features[currentIdx].CheckIsUseless(cardDrew, Discards) && (steptohu >= pre_steptohu))//和無法降低進聽數
				{
					if (features[currentIdx].tin || steptohu <= 2)//或進聽數<=2
					{
						features[currentIdx].happen = 10;//摸到無用的牌
					}
					else
					{
						features[currentIdx].happen = 11;//摸到安全牌
					}
				}
			}
			log->logConversation(turn, features[currentIdx].Print2());
			log->logConversation(turn, features[currentIdx].Print(conditions));
			features[currentIdx].clear();
			features[currentIdx].move = 2;//打牌
			players[currentIdx]->Hand -= action.second;	
			features[currentIdx].AddThrowBefore(action.second);//哈拉麻將
			log->logState(turn, Log::DISCARD, action.second, currentIdx);
			throwRecord.push_back(Record(turn, players[currentIdx]->uid, action.second));

			Hand_ = features[currentIdx].HandToTiles(players[currentIdx]->Hand);//哈拉麻將
			steptohu = features[currentIdx].StepToHu1617(Hand_);
			if (steptohu == 1 && !features[currentIdx].tin)
			{
				features[currentIdx].tin = 1;
				for (int i = 0; i < 3; i++)
				{
					features[(currentIdx + 1 + i) % 4].other_tin = 1;
				}
				log->writeLog(to_string(turn) + "," + to_string(players[currentIdx]->uid) + ",聽," + to_string(players[currentIdx]->throwTimes));
			}
			
			Hola::duration++;
			turn++;
			break;
		}
		cout << "WannHuGon return wrong value" << endl;	
	}
}

long long GameManager::CardTo34(const MJCard & card) {
	long long result = 1;
	switch (card.color) {
	case 0:
		result <<= card.value;
		return result;
	case 1:
		result <<= (9 + card.value);
		return result;
	case 2:
		result <<= (18 + card.value);
		return result;
	case 3:
		result <<= (27 + card.value);
		return result;
	case 4:
		result <<= (30 + card.value);
		return result;
	default:
		cout << "CardTo34 error" << endl;
		return -1;
	}
}

long long GameManager::CardTo63(const MJCard & card) {
	long long result = 1;
	switch (card.color) {
	case 0:
		result <<= (card.value * 3);
		return result;
	case 1:
		result <<= (21 + card.value * 3);
		return result;
	case 2:
		result <<= (42 + card.value * 3);
		return result;
	
	default:
		cout << "CardTo63 error" << endl;
		return -1;
	}
}
long long GameManager::CardTo63Order(const MJCard & card, int middle) {
	long long result = 1;
	result <<= (middle + card.value * 3) * 3;
	return result;
}

void GameManager::ThrowCard() {

}

void GameManager::AnyActions(const MJCard & card) {
	if (Hola::duration > 25 || features[currentIdx].other_tin)
	{
		if (!features[currentIdx].CheckIsSafe(card, Discards))
		{
			features[currentIdx].happen = 8;//打危險牌
		}
	}
	if ((Hola::duration > 25 || features[currentIdx].other_tin) && find(features[currentIdx].safe_before.begin(), features[currentIdx].safe_before.end(), action.second) != features[currentIdx].safe_before.end())
	{
		features[currentIdx].happen = 9;//跟打
	}
	features[currentIdx].safe_before.clear();
	log->logConversation(turn, features[currentIdx].Print2());
	log->logConversation(turn, features[currentIdx].Print(conditions));
	int temp = rand() % 3;//別的玩家對跟打或打危險牌說話
	features[(currentIdx + 1 + temp) % 4].happen = features[currentIdx].happen;
	log->logConversation(turn, features[(currentIdx + 1 + temp) % 4].Print(conditions));
	features[(currentIdx + 1 + temp) % 4].clear();
	

	if (card.color == -1) {
		cout << "Any action error" << endl;
		currentIdx = (currentIdx + 1) % 4;
		moPie = true;
		return;
	}
	if (card.color == 5) {
		return;
	}
	for (int i = 0; i < 3; i++) {
		canHu = CheckHu((currentIdx + 1 + i) % 4, card);
		canGon = CheckGon((currentIdx + 1 + i) % 4, card);
		canPon = CheckPon((currentIdx + 1 + i) % 4, card);

		if (i == 0 && card.color <= 2) {
			canEat = CheckEat((currentIdx + 1 + i) % 4, card);
		}
		else {
			canEat = false;
		}
		
		if (canHu || canGon.first || canPon || canEat) {
			HGPE[i] = players[(currentIdx + 1 + i) % 4]->WannaHGPE(canHu, canGon.first, canPon, canEat, card, currentIdx);
			if (HGPE[i] == COMMAND_GON)//哈拉麻將
			{
				features[(currentIdx + 1 + i) % 4].times[2].times++;
				features[(currentIdx + 1 + i) % 4].Happen(HGPE[i]);
			}
			else if (HGPE[i] == COMMAND_PON)
			{
				if (features[currentIdx].CheckLastNeed(*players[(currentIdx + 1 + i) % 4], HGPE[i], card, Discards))
				{
					features[(currentIdx + 1 + i) % 4].last_card = 1;
				}

				features[(currentIdx + 1 + i) % 4].times[1].times++;
				features[(currentIdx + 1 + i) % 4].Happen(HGPE[i]);
			}
			else if (HGPE[i] == COMMAND_EAT)
			{
				if (features[currentIdx].CheckLastNeed(*players[(currentIdx + 1 + i) % 4], HGPE[i], card, Discards))
				{
					features[(currentIdx + 1 + i) % 4].last_card = 1;
				}

				features[(currentIdx + 1 + i) % 4].times[0].times++;
				features[(currentIdx + 1 + i) % 4].Happen(HGPE[i]);
			}
			else if (HGPE[i] == NONE)
			{
				features[(currentIdx + 1 + i) % 4].take_action = 0;
				features[(currentIdx + 1 + i) % 4].Happen(HGPE[i]);
			}
		}
		else {
			HGPE[i] = NONE;
			features[(currentIdx + 1 + i) % 4].Happen(HGPE[i]);//哈拉麻將
		}
	}

	if (HGPE[0] == COMMAND_HU || HGPE[1] == COMMAND_HU || HGPE[2] == COMMAND_HU) {
		features[currentIdx].happen = 14;//被胡
		if (HGPE[0] == COMMAND_HU) {
			features[(currentIdx + 1) % 4].happen = 14;//哈拉麻將 胡
			gameOver = true;
			PrintTai((currentIdx + 1) % 4, card, false, winds, numKeepWin, currentIdx);
	
			if (dronIdx != (currentIdx + 1) % 4) {
				keepWin = false;
				numKeepWin = 0;
			}
			else {
				keepWin = true;
				numKeepWin++;
			}
			return;
		}
		else if (HGPE[1] == COMMAND_HU) {
			features[(currentIdx + 2) % 4].happen = 14;//哈拉麻將 胡
			gameOver = true;
			PrintTai((currentIdx + 2) % 4, card, false, winds, numKeepWin, currentIdx);
			
			if (dronIdx != (currentIdx + 2) % 4) {
				keepWin = false;
				numKeepWin = 0;
			}
			else {
				keepWin = true;
				numKeepWin++;
			}
			return;
		}
		else if (HGPE[2] == COMMAND_HU) {
			features[(currentIdx + 2) % 4].happen = 14;//哈拉麻將 胡
			gameOver = true;
			PrintTai((currentIdx + 3) % 4, card, false, winds, numKeepWin, currentIdx);
			
			if (dronIdx != (currentIdx + 3) % 4) {
				keepWin = false;
				numKeepWin = 0;
			}
			else {
				keepWin = true;
				numKeepWin++;
			}
			return;
		}
	}
	
	for (int i = 0; i < 4; i++)//哈拉麻將
	{
		features[i].safe_before.push_back(card);
	}

	for (int i = 0; i < 3; i++) {
		if (HGPE[i] == COMMAND_GON) {

			players[(currentIdx + 1 + i) % 4]->Hand -= card;
			players[(currentIdx + 1 + i) % 4]->Hand -= card;
			players[(currentIdx + 1 + i) % 4]->Hand -= card;
			players[(currentIdx + 1 + i) % 4]->gon |= CardTo34(card);
			Discards += card;
			Discards += card;
			Discards += card;
			Discards += card;
			if (HGPE[0] == COMMAND_EAT)//哈拉麻將
			{
				features[(currentIdx + 1) % 4].CantEat();
			}
			features[currentIdx].BaHappen(HGPE[i]);//哈拉麻將
			log->logState(turn, Log::KONG, card, (currentIdx + 1 + i) % 4);
			log->logConversation(turn, features[currentIdx].Print2());
			for (int i = 0; i < 4; i++)
			{
				log->logConversation(turn, features[i].Print(conditions));
				features[i].clear();
			}

			currentIdx = (currentIdx + 1 + i) % 4;
			//Hola::duration++;
			moPie = true;
			return;
		}
		else if (HGPE[i] == COMMAND_PON) {

			players[(currentIdx + 1 + i) % 4]->Hand -= card;
			players[(currentIdx + 1 + i) % 4]->Hand -= card;
			players[(currentIdx + 1 + i) % 4]->pon |= CardTo34(card);
			Discards += card;
			Discards += card;
			Discards += card;
			
			if (HGPE[0] == COMMAND_EAT)//哈拉麻將
			{
				features[(currentIdx + 1) % 4].CantEat();
			}
			features[currentIdx].BaHappen(HGPE[i]);//哈拉麻將
			log->logState(turn, Log::PONG, card, (currentIdx + 1 + i) % 4);
			log->logConversation(turn, features[currentIdx].Print2());
			for (int i = 0; i < 4; i++)
			{
				log->logConversation(turn, features[i].Print(conditions));
				features[i].clear();
			}
			action.first = COMMAND_PON;
			action.second = players[(currentIdx + 1 + i) % 4]->Throw();

			players[(currentIdx + 1 + i) % 4]->Hand -= action.second;

			features[(currentIdx + 1 + i) % 4].move = 2;
			features[(currentIdx + 1 + i) % 4].AddThrowBefore(action.second);//哈拉麻將
			players[(currentIdx + 1 + i) % 4]->throwTimes++;
			log->logState(turn, Log::DISCARD, action.second, (currentIdx + 1 + i) % 4);
			throwRecord.push_back(Record(turn, players[(currentIdx + 1 + i) % 4]->uid, action.second));

			Tiles_ Hand_ = features[(currentIdx + 1 + i) % 4].HandToTiles(players[(currentIdx + 1 + i) % 4]->Hand);//哈拉麻將
			int steptohu = features[(currentIdx + 1 + i) % 4].StepToHu1617(Hand_);
			if (steptohu == 1 && !features[(currentIdx + 1 + i) % 4].tin)
			{
				features[(currentIdx + 1 + i) % 4].tin = 1;
				for (int j = 0; j < 3; j++)
				{
					features[((currentIdx + 1 + i) + 1 + j) % 4].other_tin = 1;
				}

				log->writeLog(to_string(turn) + "," + to_string(players[(currentIdx + 1 + i) % 4]->uid) + ",聽," + to_string(players[(currentIdx + 1 + i) % 4]->throwTimes));
			}
			Hola::duration++;
			turn++;
			currentIdx = (currentIdx + 1 + i) % 4;
			moPie = false;
			return;
		}
	}

	if (HGPE[0] == COMMAND_EAT) {

		int temp;

		temp = players[(currentIdx + 1) % 4]->Pick2Eat(card); //0代表用左邊兩張吃 1代表中洞吃 2代表右邊兩張吃

		if (temp == 0)
		{
			players[(currentIdx + 1) % 4]->Hand -= MJCard(card.color, card.value - 2);
			players[(currentIdx + 1) % 4]->Hand -= MJCard(card.color, card.value - 1);
			players[(currentIdx + 1) % 4]->eat += CardTo63(MJCard(card.color, card.value - 2));
			players[(currentIdx + 1) % 4]->eatOrder[card.color] += CardTo63Order(MJCard(card.color, card.value - 2), 1);
			Discards += card;
			Discards += MJCard(card.color, card.value - 2);
			Discards += MJCard(card.color, card.value - 1);
		}
		else if (temp == 1) {
			players[(currentIdx + 1) % 4]->Hand -= MJCard(card.color, card.value + 1);
			players[(currentIdx + 1) % 4]->Hand -= MJCard(card.color, card.value - 1);
			players[(currentIdx + 1) % 4]->eat += CardTo63(MJCard(card.color, card.value - 1));
			players[(currentIdx + 1) % 4]->eatOrder[card.color] += CardTo63Order(MJCard(card.color, card.value - 1), 0);
			Discards += card;
			Discards += MJCard(card.color, card.value + 1);
			Discards += MJCard(card.color, card.value - 1);
		}
		else if (temp == 2) {
			players[(currentIdx + 1) % 4]->Hand -= MJCard(card.color, card.value + 1);
			players[(currentIdx + 1) % 4]->Hand -= MJCard(card.color, card.value + 2);
			players[(currentIdx + 1) % 4]->eat += CardTo63(card);
			players[(currentIdx + 1) % 4]->eatOrder[card.color] += CardTo63Order(card, 2);
			Discards += card;
			Discards += MJCard(card.color, card.value + 2);
			Discards += MJCard(card.color, card.value + 1);
		}
		else {
			cout << "Pick2Eat return wrong value" << endl;
		}
		features[currentIdx].BaHappen(HGPE[0]);//哈拉麻將
		log->logState(turn, Log::CHOW, card, (currentIdx + 1) % 4);
		log->logConversation(turn, features[currentIdx].Print2());
		for (int i = 0; i < 4; i++)
		{
			log->logConversation(turn, features[i].Print(conditions));
			features[i].clear();
		}
		
		action.first = COMMAND_HU; //debug用
		action.second = players[(currentIdx + 1) % 4]->Throw();
		players[(currentIdx + 1) % 4]->Hand -= action.second;

		features[(currentIdx + 1) % 4].move = 2;
		features[(currentIdx + 1) % 4].AddThrowBefore(action.second);//哈拉麻將
		players[(currentIdx + 1) % 4]->throwTimes++;
		log->logState(turn, Log::DISCARD, action.second, (currentIdx + 1) % 4);
		throwRecord.push_back(Record(turn, players[(currentIdx + 1) % 4]->uid, action.second));

		Tiles_ Hand_ = features[(currentIdx + 1) % 4].HandToTiles(players[(currentIdx + 1) % 4]->Hand);//哈拉麻將
		int steptohu = features[(currentIdx + 1) % 4].StepToHu1617(Hand_);
		if (steptohu == 1 && !features[(currentIdx + 1) % 4].tin)
		{
			features[(currentIdx + 1) % 4].tin = 1;
			for (int j = 0; j < 3; j++)
			{
				features[((currentIdx + 1) + 1 + j) % 4].other_tin = 1;
			}

			log->writeLog(to_string(turn) + "," + to_string(players[(currentIdx + 1) % 4]->uid) + ",聽," + to_string(players[(currentIdx + 1) % 4]->throwTimes));
		}
		Hola::duration++;
		turn++;
		currentIdx = (currentIdx + 1) % 4;
		moPie = false;
		return;
	}

	
	features[currentIdx].BaHappen(NONE);
	features[currentIdx].clear();
	Discards += card;
	currentIdx = (currentIdx + 1) % 4;
	
	moPie = true;
}

bool GameManager::CheckHu(int index, const MJCard & card) {
	const auto currPlayer = players[index];
	info.doors[0] = info.doors[1] = info.doors[2] = info.doors[3] = info.certainTile = 0;

	MJCards _hand(currPlayer->Hand);
	_hand += card;
	info.hands[0] = _hand.mjcolor[0].value;
	info.hands[1] = _hand.mjcolor[1].value;
	info.hands[2] = _hand.mjcolor[2].value;
	info.hands[3] = _hand.cfbCards << 12;
	info.hands[3] |= _hand.windsCards;

	EatConvert(currPlayer->eat);
	PonConvert(currPlayer->pon);
	GonConvert(currPlayer->gon);
	GonConvert(currPlayer->anGon);
	CertainTileConvert(card);
	if (currPlayer->pon != 0 || currPlayer->gon != 0 || currPlayer->anGon != 0)
		info.allChow = false;
	if (currPlayer->eat != 0)
		info.allPon = false;
	if (currPlayer->eat != 0 || currPlayer->pon != 0 || currPlayer->gon != 0)
		info.isClean = false;
	info.isZimo = false;
	info.noBonus = false;
	info.winds.all = 0b1100;

	const auto [tai, str] = calc.countTai(info);
	//cout << ScoreCalc::huInfoToString(info) << endl

	if (tai > -1) {
		/*cout << "已胡牌 台: " << tai << " " << "牌型: " << str << endl;
		logp->NewTurn(turn);
		*(logp->outFile) << ",胡,牌: " << card.color << " " << card.value << ",Player " << (currentIdx ) % 4 << players[(currentIdx ) % 4]->Print() << endl;*/
		return true;
	}

	return false;
}

pair<bool, MJCard> GameManager::CheckGon(int index, const MJCard & card, bool myDraw) {
	

	//別人打的
	if (!myDraw) {
		if (players[index]->Hand.Count(card) == 3) {
			return make_pair(true, card);
		}

	}
	//自己摸的
	else {

		//槓摸進來的那張
		if (players[index]->Hand.Count(card) == 3) {
			return make_pair(true, card);
		}
		else if (players[index]->pon & CardTo34(card)) {
			return make_pair(true, card);
		}

		//延遲槓

		//萬筒條
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 9; j++) {
				if (players[index]->Hand.Count(MJCard(i, j)) == 4) {
					return make_pair(true, MJCard(i, j));
				}
				else if (players[index]->Hand.Count(MJCard(i, j)) == 1 && (players[index]->pon & CardTo34(MJCard(i, j)))) {
					return make_pair(true, MJCard(i, j));
				}
			}
		}

		//中發白
		for (int i = 0; i < 3; i++) {
			if (players[index]->Hand.Count(MJCard(3, i)) == 4) {
				return make_pair(true, MJCard(3, i));
			}
			else if (players[index]->Hand.Count(MJCard(3, i)) == 1 && (players[index]->pon & CardTo34(MJCard(3, i)))) {
				return make_pair(true, MJCard(3, i));
			}
		}

		//風牌
		for (int i = 0; i < 4; i++) {
			if (players[index]->Hand.Count(MJCard(4, i)) == 4) {
				return make_pair(true, MJCard(4, i));
			}
			else if (players[index]->Hand.Count(MJCard(4, i)) == 1 && (players[index]->pon & CardTo34(MJCard(4, i)))) {
				return make_pair(true, MJCard(4, i));
			}
		}

	}
	return make_pair(false, MJCard(-1, -1));
}

bool GameManager::CheckPon(int index, const MJCard & card) {
	if (players[index]->Hand.Count(card) >= 2)
		return true;
	else
		return false;
}

bool GameManager::CheckEat(int index, const MJCard & card) {
	

	if (card.color > 2 || card.color == -1)
	{
		cout << "Check Eat error" << endl;
		return false;
	}

	//看是否是 1 9
	if (card.value == 0) {
		if (players[index]->Hand.Count(MJCard(card.color, card.value + 1)) >= 1 && players[index]->Hand.Count(MJCard(card.color, card.value + 2)) >= 1)
			return true;
	}
	else if (card.value == 8) {
		if (players[index]->Hand.Count(MJCard(card.color, card.value - 1)) >= 1 && players[index]->Hand.Count(MJCard(card.color, card.value - 2)) >= 1)
			return true;
	}

	//2 8
	else if (card.value == 1) {
		if (players[index]->Hand.Count(MJCard(card.color, card.value - 1)) >= 1 && players[index]->Hand.Count(MJCard(card.color, card.value + 1)) >= 1)
			return true;
		if (players[index]->Hand.Count(MJCard(card.color, card.value + 1)) >= 1 && players[index]->Hand.Count(MJCard(card.color, card.value + 2)) >= 1)
			return true;
	}
	else if (card.value == 7) {
		if (players[index]->Hand.Count(MJCard(card.color, card.value - 1)) >= 1 && players[index]->Hand.Count(MJCard(card.color, card.value + 1)) >= 1)
			return true;
		if (players[index]->Hand.Count(MJCard(card.color, card.value - 1)) >= 1 && players[index]->Hand.Count(MJCard(card.color, card.value - 2)) >= 1)
			return true;
	}


	else {
		if (players[index]->Hand.Count(MJCard(card.color, card.value - 1)) >= 1 && players[index]->Hand.Count(MJCard(card.color, card.value + 1)) >= 1)
			return true;
		if (players[index]->Hand.Count(MJCard(card.color, card.value + 1)) >= 1 && players[index]->Hand.Count(MJCard(card.color, card.value + 2)) >= 1)
			return true;
		if (players[index]->Hand.Count(MJCard(card.color, card.value - 1)) >= 1 && players[index]->Hand.Count(MJCard(card.color, card.value - 2)) >= 1)
			return true;
	}

	return false;
}

void GameManager::Init() {
	Sea = MJCards(true);
	Discards = MJCards();
	HuCards = MJCards();
	//GameLog::RoundCount = 0;
}

void GameManager::InitPlayer() {
	int len = Sea.Count();
	for (int i = 0; i < 4; i++) {
		int newIdx = i + rand() % (4 - i);
		std::swap(players[i], players[newIdx]);
	}
	for (int i = 0; i < 4; i++) {
		
		for (int j = 0; j < 16; j++) {
			int idx = rand() % len;
			MJCard result = Sea.at(idx);
			Sea -= result;
			players[i]->Hand += result;
			len -= 1;
		}
	}
}

void GameManager::EatConvert(const long long & eat) {
	//long long temp = eat;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 7; j++) {
			int temp;
			temp = (eat >> (i * 21 + j * 3)) & 7;
			for (int k = 0; k < temp; k++) {
				info.doors[i] += (73 << (j * 3));
			}
		}
	}
	
}

void GameManager::PonConvert(const long long & pon) {
	//long long temp = eat;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			int temp;
			temp = (pon >> (i * 9 + j )) & 1;
			if (temp) {
				info.doors[i] += (3 << j * 3);
			}
		}
	}

	for (int j = 0; j < 4; j++) {
		int temp;
		temp = (pon >> (30 + j)) & 1;
		if (temp) {
			info.doors[3] += (3 << j * 3);
		}
	}

	for (int j = 0; j < 3; j++) {
		int temp;
		temp = (pon >> (27 + j)) & 1;
		if (temp) {
			info.doors[3] += (3 << 12 + j * 3);
		}
	}

}

void GameManager::GonConvert(const long long & gon, bool min) {
	//long long temp = eat;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			int temp;
			temp = (gon >> (i * 9 + j)) & 1;
			if (temp) {
				info.doors[i] += (4 << j * 3);
			}
		}
	}

	for (int j = 0; j < 4; j++) {
		int temp;
		temp = (gon >> (30 + j)) & 1;
		if (temp) {
			info.doors[3] += (4 << j * 3);
		}
	}

	for (int j = 0; j < 3; j++) {
		int temp;
		temp = (gon >> (27 + j)) & 1;
		if (temp) {
			info.doors[3] += (4 << 12 + j * 3);
		}
	}

}

void GameManager::CertainTileConvert(const MJCard & card) {
	
	switch (card.color) {
	case 0:
		
		info.certainTile = card.value;
		return;
	case 1:
		info.certainTile = card.value + 9;
		return;
	case 2:
		info.certainTile = card.value + 18;
		return;
	case 3:
		info.certainTile = card.value + 31;
		return;
	case 4:
		info.certainTile = card.value + 27;
		return;
	
	default:
		cout << "GM CardTo34 error" << endl;
		return;
	}
}

bool GameManager::CheckGonHu(const MJCard & card, int currentIdx) {
	for (int i = 1; i < 4; i++) {
		if (CheckHu((currentIdx + i) % 4, card)) {
			if (players[(currentIdx + i) % 4]->WannaHu(card)) {
				gameOver = true;

				PrintTai((currentIdx + i) % 4, card, false, winds, numKeepWin, currentIdx, true);
				
				if (dronIdx != (currentIdx + i) % 4) {
					keepWin = false;
					numKeepWin = 0;
				}
				else {
					keepWin = true;
					numKeepWin++;
				}
				return true;
			}
		}
	}
	return false;
}

void GameManager::PrintTai(int playeridx, const MJCard & card, bool zimo, int wind, int numKeepWin, int pao, bool gonHu, bool lastCard, bool sevenHuas) {
	if (zimo) {
		log->logState(turn, Log::ZIMO, card, playeridx);
		log->logConversation(turn, features[currentIdx].Print2());
		log->logConversation(turn, features[currentIdx].Print(conditions));
	}
	else {
		log->logState(turn, Log::HU, card, playeridx);
		log->logConversation(turn, features[currentIdx].Print2());
		for (int i = 0; i < 4; i++)
		{
			log->logConversation(turn, features[i].Print(conditions));
		}
	}
	
	if (sevenHuas) {
		if (zimo) {
			if (playeridx == dronIdx) {
				const int s = BASE + (8 + numKeepWin * 2 + 1) * TAI;
				score[uid[playeridx]] += s * 3;
				score[(uid[playeridx] + 1) % 4] -= s;
				score[(uid[playeridx] + 2) % 4] -= s;
				score[(uid[playeridx] + 3) % 4] -= s;
				log->logResult(8 + (numKeepWin * 2 + 1), 3 * (8 + (numKeepWin * 2 + 1)), "莊家連" + to_string(numKeepWin) + " 八仙過海");
			}
			else {
				score[uid[playeridx]] += 8 * 3 + (numKeepWin * 2 + 1) * TAI;
				score[(uid[playeridx] + 1) % 4] -= 8;
				score[(uid[playeridx] + 2) % 4] -= 8;
				score[(uid[playeridx] + 3) % 4] -= 8;
				score[uid[dronIdx]] -= (numKeepWin * 2 + 1)* TAI;
				log->logResult(8, 24 + (numKeepWin * 2 + 1), "八仙過海 ");
			}
		}
		else {
			int s = 0;
			if (playeridx != dronIdx && pao != dronIdx) {
				numKeepWin = 0;
				s = BASE + 8 * TAI;
				log->logResult(8, 8, "七搶一");
			}
			else if (playeridx == dronIdx) {
				s = BASE + (8 + numKeepWin * 2 + 1) * TAI;
				log->logResult(8 + (numKeepWin * 2 + 1), 8 + (numKeepWin * 2 + 1), "莊家連" + to_string(numKeepWin) + " 七搶一 ");
			}
			else if (pao == dronIdx) {
				s = BASE + (8 + numKeepWin * 2 + 1) * TAI;
				log->logResult(8, 8 + (numKeepWin * 2 + 1), "七搶一");
			}

			score[uid[playeridx]] += s;
			score[uid[pao]] -= s;
		}
		return;
	}
	
	const auto player = players[playeridx];
	info.doors[0] = info.doors[1] = info.doors[2] = info.doors[3] = info.certainTile = 0;

	MJCards _hand(player->Hand);
	_hand += card;
	info.hands[0] = _hand.mjcolor[0].value;
	info.hands[1] = _hand.mjcolor[1].value;
	info.hands[2] = _hand.mjcolor[2].value;
	info.hands[3] = _hand.cfbCards << 12;
	info.hands[3] |= _hand.windsCards;

	EatConvert(player->eat);
	PonConvert(player->pon);
	GonConvert(player->gon);
	GonConvert(player->anGon);
	CertainTileConvert(card);

	//無碰槓
	info.allChow = !player->pon && !player->gon && !player->anGon;
	//無吃
	info.allPon = !player->eat;
	//無吃碰槓
	info.isClean = !player->eat && !player->pon && !player->gon;
	//無花
	info.noBonus = !player->flowers;
	info.isZimo = zimo;
	
	info.winds.all = wind;

	auto [tai, str] = calc.countTai(info);
	
	if (!zimo) {
		if (gonHu) {
			tai++;
			str += "搶槓胡 ";
		}

		int s = 0;
		if (playeridx != dronIdx && pao != dronIdx) {
			numKeepWin = 0;
			s = BASE + tai * TAI;
		}
		else {
			s = BASE + (tai + numKeepWin * 2 + 1) * TAI;
		}

		score[uid[playeridx]] += s;
		score[uid[pao]] -= s;
	}
	else {
		if (lastCard) {
			tai++;
			str += "海底撈月 ";
		}
		if (playeridx == dronIdx) {
			const int dealerScore = BASE + (tai + numKeepWin * 2 + 1) * TAI;

			score[uid[playeridx]] += dealerScore * 3;
			score[(uid[playeridx] + 1) % 4] -= dealerScore;
			score[(uid[playeridx] + 2) % 4] -= dealerScore;
			score[(uid[playeridx] + 3) % 4] -= dealerScore;
		}
		else {
			const int s = BASE + tai * TAI;

			score[uid[playeridx]] += s * 3 + (numKeepWin * 2 + 1) * TAI;
			score[(uid[playeridx] + 1) % 4] -= s;
			score[(uid[playeridx] + 2) % 4] -= s;
			score[(uid[playeridx] + 3) % 4] -= s;
			score[uid[dronIdx]] -= (numKeepWin * 2 + 1) * TAI;
		}
	}
	if (zimo) {
		if (playeridx == dronIdx) {
			str += "莊家連" + to_string(numKeepWin) + " ";
			log->logResult(tai + (numKeepWin * 2 + 1), 3 * (tai + (numKeepWin *2 + 1)), str);
		}
		else {
			log->logResult(tai , 3 * tai + (numKeepWin * 2 + 1), str);
		}
	}
	else {
		if (playeridx != dronIdx && pao != dronIdx) {
			numKeepWin = 0;
			log->logResult(tai, tai, str);
		}
		else if (playeridx == dronIdx) {
			log->logResult(tai + (numKeepWin * 2 + 1), tai + (numKeepWin * 2 + 1), "莊家連" + to_string(numKeepWin) + str);
		}
		else if (pao == dronIdx) {
			log->logResult(tai, tai + (numKeepWin * 2 + 1), str);
		}
	}
	

	//log->logResult(tai + (numKeepWin * 2 + 1), str);

	if (tai == -1) {
		cout << "PrintTai Error" << endl;
	}
}

bool GameManager::Check7Huas(int currentIdx) {
	
	if (sevenHuas) {
		gameOver = true;
		if (sevenId != currentIdx) {
			PrintTai(sevenId, MJCard(-1,-1), false, winds, numKeepWin, currentIdx, false, false, true);
			
			if (dronIdx != sevenId) {
				keepWin = false;
				numKeepWin = 0;
			}
			else {
				keepWin = true;
				numKeepWin++;
			}
		}
		else {
			PrintTai(sevenId, MJCard(-1, -1), true, winds, numKeepWin, currentIdx, false, false, true);

			if (dronIdx != sevenId) {
				keepWin = false;
				numKeepWin = 0;
			}
			else {
				keepWin = true;
				numKeepWin++;
			}
		}
		return true;
	}
	else if (FlowersCount(players[currentIdx]->flowers) == 7) {
		sevenHuas = true;
		sevenId = currentIdx;
	}
	return false;
}

int GameManager::FlowersCount(int flowers) {
	int result = 0;
	for (int i = 0; i < 8; i++) {
		result += ((flowers >> i) & 1);
	}
	return result;
}



