#include "PacmanDTController.h"

PacmanDTController::PacmanDTController(std::shared_ptr<Character> character):
	Controller(character){
}

PacmanDTController::~PacmanDTController() {

}

Move
PacmanDTController::getMove(const GameState& game){
	
	int pacmanNode = character->getPos();
	auto pacmanCoords = game.getMaze().getNodePos(pacmanNode);
	
    int nearestGhost = 0;
    int minGhostDist = 10000000;

    for (int i = 0; i < 4; i++){ //buscamos el fantasma más cercano de los 4 que hay
        auto ghostCoords = game.getMaze().getNodePos(game.getGhostsPos(i));
        ghostCoords.first -= pacmanCoords.first;
        ghostCoords.second -= pacmanCoords.second;

        int sqDist = ghostCoords.first*ghostCoords.first+ghostCoords.second*ghostCoords.second; //calculamos la distancia
        if(sqDist < minGhostDist){
            minGhostDist = sqDist;
            nearestGhost = i;
        }
    }
    
    auto targetCoords = game.getMaze().getNodePos(game.getGhostsPos(nearestGhost)); //tomamos las cordenadas del fantasma más cercano
    std::vector<Move> moves = game.getMaze().getPossibleMoves(pacmanNode); //movimientos legales de pacman

	//Evitar que Pacman se "trabe" invirtiendo su dirección.
	//Solo se permite revertir si es la única opción disponible (ej. un callejón sin salida).
    if (moves.size() > 1){
        Move reverse = turnBack(character->getDirection());
        moves.erase(std::remove(moves.begin(), moves.end(), reverse), moves.end());
        if (moves.empty()){
            moves.push_back(reverse); //no había otra opción real, se permite igual
        }
    }

    if (game.isGhostEdible(nearestGhost)){
        int minDist = 1000000;
        Move minMove=character->getDirection();
        for(Move m:moves){
            int vecino = game.getMaze().getNeighbour(pacmanNode,m);
			
			if(vecino<0)
				continue;

			auto vecinoCoords = game.getMaze().getNodePos(vecino);
			vecinoCoords.first-=targetCoords.first;
			vecinoCoords.second-=targetCoords.second;
			int sqDist=vecinoCoords.first*vecinoCoords.first+vecinoCoords.second*vecinoCoords.second;
			if(sqDist<minDist){
				minDist=sqDist;
				minMove=m;
			}
		}
        return minMove;
    }
    else{

        int maxDist=-1;
		Move maxMove=character->getDirection();
		for(Move m:moves){
			int vecino = game.getMaze().getNeighbour(pacmanNode,m);
			if(vecino<0)continue;
			auto vecinoCoords = game.getMaze().getNodePos(vecino);
			vecinoCoords.first-=targetCoords.first;
			vecinoCoords.second-=targetCoords.second;
			int sqDist=vecinoCoords.first*vecinoCoords.first+vecinoCoords.second*vecinoCoords.second;
			if(sqDist>maxDist){
				maxDist=sqDist;
				maxMove=m;
			}
		}
		return maxMove;
    }
}