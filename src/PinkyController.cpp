#include "PinkyController.h"


PinkyInfo* PinkyInfo:: info = nullptr; //solo existe una instancia

//Arma el árbol una vez en el constructor
PinkyController::PinkyController(std::shared_ptr<Character> character):
	Controller(character), root(std::make_shared<Selector>()){
	
	//1ra Rama: Si Pinky es comestible, huir
	auto filter = std::make_shared<Filter>();
	filter->addCondition(std::make_shared<PinkyPowerPill>()); //Condición
	filter->addAction(std::make_shared<PinkyFrightened>()); //Acción
	root->addChild(filter);

	//2da Rama: si no está en frightened, elegir entre Scatter y Chase
	auto nonFrightened = std::make_shared<Selector>();

	auto scatterFilter = std::make_shared<Filter>();
	scatterFilter->addCondition(std::make_shared<PinkyTimeOut>()); //Condición
	scatterFilter->addAction(std::make_shared<PinkyScatter>()); //Acción
	nonFrightened->addChild(scatterFilter);

	nonFrightened->addChild(std::make_shared<PinkyChase>());

	root->addChild(nonFrightened);
}

PinkyController::~PinkyController() {
}

Move
PinkyController::getMove(const GameState& game){
	//dejamos en la pizarra el estado del juego y a quien controlamos
	//tickeamos el arbol completo y devolvemos el movimiento que haya 
	//quedado escrito por la acción que se ejecuta
	PinkyInfo::getInfo()->in_character = character;
	PinkyInfo::getInfo()->in_gamestate = &game;
	root->tick();

	return PinkyInfo::getInfo()->out_move;
}

PinkyTimeOut::PinkyTimeOut() : Behavior() {
	lastTime = std::chrono::high_resolution_clock::now();
}

Status PinkyTimeOut::update(){
	std::chrono::duration<float> timeStamp = std::chrono::high_resolution_clock::now() - lastTime;
	if ((int)timeStamp.count()%27 < 7){
		return BH_SUCCESS;
	}else{
		return BH_FAILURE;
	}
}

Status PinkyChase::update(){
	auto character = PinkyInfo::getInfo()->in_character;
	auto gs = PinkyInfo::getInfo()->in_gamestate;

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
	
	PinkyInfo::getInfo()->out_move = minMove;
	return BH_SUCCESS;
}

Status PinkyPowerPill::update(){
	auto character = PinkyInfo::getInfo()->in_character;
	auto ghost = dynamic_cast<Ghost*>(character.get());

	if (ghost!=nullptr && ghost->isEdible()){
		return BH_SUCCESS;
	}else{
		return BH_FAILURE;
	}
}

Status PinkyFrightened::update(){
	auto character = PinkyInfo::getInfo()->in_character;
	auto gs = PinkyInfo::getInfo()->in_gamestate;
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

	PinkyInfo::getInfo()->out_move = maxMove;
	return BH_SUCCESS;
}

PinkyScatter::PinkyScatter(): Behavior(){
	//esquina arriba a la derecha, se elige un x gigante en vez del
	//ancho real del laberinto para no tener que calcularlo. Al solo comparar
	//distancias con el punto, el efecto es el mismo
	cornerTarget = std::make_pair(100000, 0);
}

Status PinkyScatter::update(){
	auto character = PinkyInfo::getInfo()->in_character;
	auto gs = PinkyInfo::getInfo()->in_gamestate;

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

	PinkyInfo::getInfo()->out_move = minMove;
	return BH_SUCCESS;
}