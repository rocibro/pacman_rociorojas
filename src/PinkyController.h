#pragma once

#include "Controller.h"
#include "BehaviorTree.h" //nodos del árbol: Behavior, Selector, Filter
#include <chrono>

class PinkyInfo{ //Pizarra compartida entre los nodos del árbol Pinky (como info de BTGhostController)
    static PinkyInfo *info;
    PinkyInfo(){}

public:
    static PinkyInfo* getInfo(){
        if(info==nullptr) info = new PinkyInfo();
        return info;
    }
    const GameState* in_gamestate;
    Move out_move;
    std::shared_ptr<Character> in_character;
};

class PinkyController: public Controller {

private: 
	std::shared_ptr<Composite> root; //raíz del árbol (un Selector)
public:
	PinkyController(std::shared_ptr<Character> character);
	virtual ~PinkyController();
	virtual Move getMove(const GameState& game)override;
};

class PinkyChase : public Behavior{ //Acción: Perseguir a Pacman apuntando a 4 casillas por delante
public:
	virtual Status update() override;
};
//Acción: modo Frightened. Elige el movimiento que deja a 
//Pinky más lejos de PacMan.
class PinkyFrightened : public Behavior{ 
public:
	virtual Status update() override;
};
//Acción: Modo Scatter. Va hacia su esquina
class PinkyScatter : public Behavior{
private:
	std::pair<int, int> cornerTarget;
public:
	virtual Status update() override;
	PinkyScatter();
};

//Condición: ¿Pinky está comestible ahora mismo? 
class PinkyPowerPill : public Behavior{
public:
	virtual Status update() override;
};

//Condición: alterna 7 segundos de Scatter con 20 segundos de Chase
class PinkyTimeOut : public Behavior{
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
public:
	virtual Status update() override;
	PinkyTimeOut();
};