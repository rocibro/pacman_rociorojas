/*
 * FSMController.cpp
 *
 *  Created on: Apr 23, 2018
 *      Author: nbarriga
 */

#include "FSMController.h"
#include <iostream>

FSMController::FSMController(std::shared_ptr<Character> character):
	Controller(character){
	buildFSM();
}

FSMController::~FSMController() {
	// TODO Auto-generated destructor stub
}
std::shared_ptr<FSMState> FSMController::createChaseState(){
	return std::make_shared<ChaseState>(character);
}

std::shared_ptr<FSMState> FSMController::createScatterState(){
	return std::make_shared<ScatterState>(character);
}

std::shared_ptr<FSMState> FSMController::createFrightenedState(){
	return std::make_shared<FrightenedState>(character);
}

void FSMController::buildFSM(){
	fsm = std::make_shared<GhostStateMachine>(character,
			createChaseState(), createScatterState(), createFrightenedState());
}

Move 
FSMController::getMove(const GameState& game){
	return fsm->update(game);
}


///////////////////////////////////PillTransition///////////////////////////////
PillTransition::PillTransition(std::shared_ptr<FSMState> next):last(0),_next(next){

}

bool PillTransition::isValid(const GameState& gs){
	int quedan=gs.getMaze().getPillPositions().size();
	if(last!=quedan && quedan%20==0){
		last =quedan;
		return true;
	}
	return false;
}
std::shared_ptr<FSMState> PillTransition::getNextState(){
	return _next;
}



///////////////////////////////ChaseState///////////////////////////////////////
ChaseState::ChaseState(std::shared_ptr<Character> _character):FSMState(_character){

}
void ChaseState::onEnter(const GameState& ){
	std::dynamic_pointer_cast<Ghost>(character)->revert();
}

Move ChaseState::onUpdate(const GameState& game){
	std::vector<Move> moves;
	const auto pacmanCoord=game.getMaze().getNodePos(game.getPacmanPos());
	const auto myPos=character->getPos();
	//const auto myCoord=game.getMaze().getNodePos(myPos);

	if(character->getDirection()==PASS){
		moves=game.getMaze().getPossibleMoves(myPos);
	}else{
		moves=game.getMaze().getGhostLegalMoves(myPos,character->getDirection());
	}

	float min=euclid2(
		game.getMaze().getNodePos(game.getMaze().getNeighbour(myPos,moves[0])),
			pacmanCoord);
	int minI=0;
	for(unsigned int i=1;i<moves.size();i++){
		auto dist=euclid2(
			game.getMaze().getNodePos(game.getMaze().getNeighbour(myPos,moves[i])),
			pacmanCoord);
		if(dist<min){
			min=dist;
			minI=i;
		}
	}
	return moves[minI];
}
ChaseState::~ChaseState(){

}

///////////////////////////////ScatterState///////////////////////////////////////
ScatterState::ScatterState(std::shared_ptr<Character> _character): FSMState(_character){}

void ScatterState::onEnter(const GameState& ){
	std::dynamic_pointer_cast<Ghost>(character)->revert();
}
Move ScatterState:: onUpdate(const GameState& game){
	std::vector<Move> moves;
	int myPos = character->getPos();
	//punto de fuga
	std::pair<int,int> cornerTarget = std::make_pair(0,0); 

	if(character->getDirection()==PASS){
		moves=game.getMaze().getPossibleMoves(myPos);
	}else{
		moves=game.getMaze().getGhostLegalMoves(myPos,character->getDirection());
	}


	int minDist = 10000000;
	Move minMove = moves[0];

	for (Move m : moves){
		int vecino = game.getMaze().getNeighbour(myPos, m);
		if (vecino < 0) continue;

		auto vecinoCoords = game.getMaze().getNodePos(vecino);
		int dx = vecinoCoords.first - cornerTarget.first;
		int dy = vecinoCoords.second - cornerTarget.second;

		int dist = dx*dx + dy*dy;

		if (dist < minDist){
			minDist = dist;
			minMove = m;
		}
	}
	return minMove;
}

ScatterState::~ScatterState(){}

///////////////////////////////NonFrightenedState///////////////////////////////////////
NonFrightenedState::NonFrightenedState(std::shared_ptr<Character> _character,
		std::shared_ptr<FSMState> _chaseState,
		std::shared_ptr<FSMState> _scatterState): FSMState(_character),
		chaseState(_chaseState), scatterState(_scatterState){
	activeChild = scatterState; //pacman clásico arranca en Scatter
	started = false;
}

void NonFrightenedState::onEnter(const GameState& gs){
	lastSwitch = std::chrono::system_clock::now();
	activeChild = scatterState;
	activeChild->onEnter(gs);
	started = true;
}

Move NonFrightenedState::onUpdate(const GameState& gs){
	if (!started){
		lastSwitch = std::chrono::system_clock::now();
		started = true;
	}
	
	auto now = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed = now - lastSwitch;

	if(activeChild == scatterState && elapsed.count() > scatter_seconds){
		activeChild->onExit(gs);
		activeChild = chaseState;
		activeChild->onEnter(gs);
		lastSwitch = now;
	}
	else if(activeChild == chaseState && elapsed.count() > chase_seconds){
		activeChild->onExit(gs);
		activeChild = scatterState;
		activeChild->onEnter(gs);
		lastSwitch = now;
	}

	return activeChild->onUpdate(gs);
}

void NonFrightenedState::onExit(const GameState& gs){
	activeChild->onExit(gs);
}

NonFrightenedState::~NonFrightenedState(){}

///////////////////////////////FrightenedState///////////////////////////////////////
FrightenedState::FrightenedState(std::shared_ptr<Character> _character):FSMState(_character){}

void FrightenedState::onEnter(const GameState&){
	std::dynamic_pointer_cast<Ghost>(character)->revert();
}

Move FrightenedState::onUpdate(const GameState& game){
	std::vector<Move> moves;
	int myPos = character->getPos();
	auto pacmanCoords = game.getMaze().getNodePos(game.getPacmanPos());

	if(character->getDirection()==PASS){
		moves=game.getMaze().getPossibleMoves(myPos);
	}else{
		moves=game.getMaze().getGhostLegalMoves(myPos,character->getDirection());
	}

	int maxDist = -1;
	Move maxMove = moves[0];

	for (Move m : moves){
		int vecino = game.getMaze().getNeighbour(myPos, m);
		if (vecino < 0) continue;
		auto vecinoCoords = game.getMaze().getNodePos(vecino);
		int dx = vecinoCoords.first - pacmanCoords.first;
		int dy = vecinoCoords.second - pacmanCoords.second;
		int dist = dx*dx + dy*dy;

		if(dist > maxDist){
			maxDist = dist;
			maxMove = m;
		}
	}

	return maxMove;
}

FrightenedState::~FrightenedState(){}

///////////////////////////////EdibleTransition///////////////////////////////////////
EdibleTransition::EdibleTransition(std::shared_ptr<Character> _character, std::shared_ptr<FSMState> _next): character(_character), next(_next){}

bool EdibleTransition::isValid(const GameState&){
	return std::dynamic_pointer_cast<Ghost>(character)->isEdible();
}

std::shared_ptr<FSMState> EdibleTransition::getNextState(){
	return next;
}

///////////////////////////////NotEdibleTransition///////////////////////////////////////
NotEdibleTransition::NotEdibleTransition(std::shared_ptr<Character> _character, std::shared_ptr<FSMState> _next): character(_character), next(_next){}

bool NotEdibleTransition::isValid(const GameState&){
	return !std::dynamic_pointer_cast<Ghost>(character)->isEdible();
}

std::shared_ptr<FSMState> NotEdibleTransition::getNextState(){
	return next;
}
/////////////////////////////////////GhostStateMachine/////////////////////////////
GhostStateMachine::GhostStateMachine(std::shared_ptr<Character> _character,
		std::shared_ptr<FSMState> chaseState,
		std::shared_ptr<FSMState> scatterState,
		std::shared_ptr<FSMState> frightenedState):FiniteStateMachine(_character){
	auto nonFrightened = std::make_shared<NonFrightenedState>(_character, chaseState, scatterState);
	auto frightened = frightenedState;

	nonFrightened->addTransition(std::make_shared<EdibleTransition>(_character, frightened));
	frightened->addTransition(std::make_shared<NotEdibleTransition>(_character, nonFrightened));

	states.push_back(nonFrightened);
	states.push_back(frightened);

	initialState = nonFrightened;
	activeState = initialState;
}


Move GhostStateMachine::update(const GameState& gs){
	auto t=activeState->getActiveTransition(gs);
	if(t!=nullptr){
		activeState->onExit(gs);
		t->onTransition(gs);
		activeState=t->getNextState();
		activeState->onEnter(gs);
	}
	return activeState->onUpdate(gs);
}


GhostStateMachine::~GhostStateMachine(){

}


