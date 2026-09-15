#include "InkyController.h"


InkyController::InkyController(std::shared_ptr<Character> character, std::shared_ptr<Character> _blinky):
	BTGhostController(character), blinky(_blinky){

	buildTree();
}

InkyController::~InkyController() {

}

std::shared_ptr<Behavior> InkyController::createChaseBehavior(){
	return std::make_shared<InkyChase>(blinky);
}

InkyChase::InkyChase(std::shared_ptr<Character> _blinky): blinky(_blinky){

}

Status InkyChase::update(){
	auto character = Info::getInfo()->in_character;
	auto gs = Info::getInfo()->in_gamestate;
	const auto &maze = gs->getMaze();

	//Punto 2 casillas por delante de Pacman en su dirección actual
	int aheadNode = gs->getPacmanPos();
	Move pacmanDir = static_cast<Move>(gs->getPacmanDir());
	if(pacmanDir != PASS){
		for(int i=0;i<2;i++){
			int next = maze.getNeighbour(aheadNode, pacmanDir);
			if(next < 0) break;
			aheadNode = next;
		}
	}
	auto aheadPos = maze.getNodePos(aheadNode);

	std::pair<int,int> target;
	if(blinky){
		//Vector desde Blinky hasta el punto de adelanto, duplicado
		auto blinkyPos = maze.getNodePos(blinky->getPos());
		int vx = aheadPos.first - blinkyPos.first;
		int vy = aheadPos.second - blinkyPos.second;
		target = std::make_pair(blinkyPos.first + 2*vx, blinkyPos.second + 2*vy);
	} else {
		//Sin referencia a Blinky: persecución directa del punto de adelanto
		target = aheadPos;
	}

	std::vector<Move> moves;
	if(character->getDirection()==PASS){
		moves = maze.getPossibleMoves(character->getPos());
	} else {
		moves = maze.getGhostLegalMoves(character->getPos(), character->getDirection());
	}

	float minDist = 10000000;
	Move minMove = moves[0];
	for(Move m : moves){
		int vecino = maze.getNeighbour(character->getPos(), m);
		if(vecino < 0) continue;
		float dist = euclid2(maze.getNodePos(vecino), target);
		if(dist < minDist){
			minDist = dist;
			minMove = m;
		}
	}

	Info::getInfo()->out_move = minMove;
	return BH_SUCCESS;
}