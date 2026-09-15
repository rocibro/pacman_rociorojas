#include "PinkyController.h"

PinkyController::PinkyController(std::shared_ptr<Character> character):
	BTGhostController(character){
	
	buildTree();
}

PinkyController::~PinkyController() {
}

std::shared_ptr<Behavior> PinkyController::createChaseBehavior(){
	return std::make_shared<PinkyChase>();
}

std::shared_ptr<Behavior> PinkyController::createFrightenedBehavior(){
	return std::make_shared<PinkyFrightened>();
}

Status PinkyChase::update(){
	auto character = Info::getInfo()->in_character;
	auto gs = Info::getInfo()->in_gamestate;

	//calculamos el nodo objetivo: 4 pasos desde Pacman en su dirección actual
	int targetNode = gs->getPacmanPos();
	Move pacmanDir = static_cast<Move>(gs->getPacmanDir());
	if(pacmanDir != PASS){
		for (int i = 0; i < 4; i++){
			int next = gs->getMaze().getNeighbour(targetNode, pacmanDir);
			if(next < 0) break; //topa con una pared, no avanza más
			targetNode = next;
		}
	}

	auto target = gs->getMaze().getNodePos(targetNode);

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

Status PinkyFrightened::update(){
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