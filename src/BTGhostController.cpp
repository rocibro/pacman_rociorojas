/*
 * BTGhostController.cpp
 *
 *  Created on: May 2, 2018
 *      Author: VIDEOJUEGOS UTALCA
 */

#include "BTGhostController.h"
#include <iostream>


Info* Info::info=nullptr;


BTGhostController::BTGhostController(std::shared_ptr<Character> character):Controller(character),root(std::make_shared<Selector>())  {
	buildTree();
}

BTGhostController::~BTGhostController() {
	// TODO Auto-generated destructor stub
}

std::shared_ptr<Behavior> BTGhostController::createChaseBehavior(){
	return std::make_shared<Chase>();
}

std::shared_ptr<Behavior> BTGhostController::createScatterBehavior(){
	return std::make_shared<Scatter>();
}

std::shared_ptr<Behavior> BTGhostController::createFrightenedBehavior(){
	return std::make_shared<Frightened>();
}

std::shared_ptr<Behavior> BTGhostController::createPowerPillCondition(){
	return std::make_shared<Powerpill>();
}

std::shared_ptr<Behavior> BTGhostController::createScatterCondition(){
	return std::make_shared<TimeOut>();
}

void BTGhostController::buildTree(){
	root = std::make_shared<Selector>();

	// 1ra rama: si soy comestible en este momento, huir
	auto filter = std::make_shared<Filter>();
	filter->addCondition(createPowerPillCondition()); 	// Condición
	filter->addAction(createFrightenedBehavior()); 		// Acción
	root->addChild(filter);

	// 2da rama: si no estoy frightened, elegir entre Scatter y Chase
	auto nonFrightened = std::make_shared<Selector>();

	auto scatterFilter = std::make_shared<Filter>();
	scatterFilter->addCondition(createScatterCondition()); 	// Condición
	scatterFilter->addAction(createScatterBehavior()); 		// Acción
	nonFrightened->addChild(scatterFilter);

	nonFrightened->addChild(createChaseBehavior());

	root->addChild(nonFrightened);
}

Move BTGhostController::getMove(const GameState& gs){
	Info::getInfo()->in_character=character;
	Info::getInfo()->in_gamestate=&gs;
	root->tick();

	return Info::getInfo()->out_move;
}

TimeOut::TimeOut() : Behavior() {
	lastTime = std::chrono::high_resolution_clock::now();

}

Status TimeOut::update(){
	std::chrono::duration<float> timeStamp = std::chrono::high_resolution_clock::now() - lastTime;
	if( (int)timeStamp.count()%27 < 7){
		return BH_SUCCESS;
	}else{
		return BH_FAILURE;
	}

}

Status Chase::update(){
	//std::cerr << " Chase \n" ;
	auto character = Info::getInfo()->in_character;
	auto gs = Info::getInfo()->in_gamestate;
	auto target= gs->getMaze().getNodePos(gs->getPacmanPos());

	std::vector<Move> moves;
	if(character->getDirection()==PASS){
		moves = gs->getMaze().getPossibleMoves(character->getPos());
	} else{
		moves = gs->getMaze().getGhostLegalMoves(character->getPos(), character->getDirection());
	}
	float minDist = 10000000;
	Move minMove = moves[0];

	for (Move m : moves){
		int vecino = gs->getMaze().getNeighbour(character->getPos(), m);
		if (vecino < 0) continue;

		auto vecinoCoords = gs->getMaze().getNodePos(vecino);
		int dx = vecinoCoords.first - target.first;
		int dy = vecinoCoords.second - target.second;

		int dist = dx*dx + dy*dy;

		if (dist < minDist){
			minDist = dist;
			minMove = m;
		}
	}
	
	Info::getInfo()->out_move = minMove;
	return BH_SUCCESS;
}

Status Powerpill::update(){
	auto character = Info::getInfo()->in_character;
	auto ghost = dynamic_cast<Ghost*>(character.get());

	if( ghost!=nullptr && ghost->isEdible()){
		return BH_SUCCESS;
	}else{
		return BH_FAILURE;
	}

}

Status Frightened::update(){
	auto character = Info::getInfo()->in_character;
	auto gs = Info::getInfo()->in_gamestate;
	auto pacman = gs->getMaze().getNodePos(gs->getPacmanPos());

	std::vector<Move> moves;
	if(character->getDirection()==PASS) {
        moves=gs->getMaze().getPossibleMoves(character->getPos());
    } else {
        moves = gs->getMaze().getGhostLegalMoves(character->getPos(), character->getDirection());
    }

	float maxDist = -1;
	Move maxMove = moves[0];
	
	for (Move m : moves){
		int vecino = gs->getMaze().getNeighbour(character->getPos(), m);
		if (vecino < 0) continue;
		auto vecinoCoords = gs->getMaze().getNodePos(vecino);
		int dx = vecinoCoords.first - pacman.first;
		int dy = vecinoCoords.second - pacman.second;
		int dist = dx*dx + dy*dy;

		if(dist > maxDist){
			maxDist = dist;
			maxMove = m;
		}
	}

	Info::getInfo()->out_move = maxMove;
	return BH_SUCCESS;
}

Scatter :: Scatter() : Behavior(){
	cornerTarget = std::make_pair(-1,-1);

}

Status Scatter::update(){
	auto character = Info::getInfo()->in_character;
	auto gs = Info::getInfo()->in_gamestate;

	std::vector<Move> moves;
	if(character->getDirection()==PASS){
		moves = gs->getMaze().getPossibleMoves(character->getPos());
	} else{
		moves = gs->getMaze().getGhostLegalMoves(character->getPos(), character->getDirection());
	}
	
	float minDist = 10000000;
	Move minMove = moves[0];

	for (Move m : moves){
		int vecino = gs->getMaze().getNeighbour(character->getPos(), m);
		if (vecino < 0) continue;

		auto vecinoCoords = gs->getMaze().getNodePos(vecino);
		int dx = vecinoCoords.first - cornerTarget.first;
		int dy = vecinoCoords.second - cornerTarget.second;

		int dist = dx*dx + dy*dy;

		if (dist < minDist){
			minDist = dist;
			minMove = m;
		}
	}

	Info::getInfo()->out_move = minMove;
	return BH_SUCCESS;

}
