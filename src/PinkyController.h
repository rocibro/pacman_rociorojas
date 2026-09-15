#pragma once

#include "BTGhostController.h"

class PinkyController: public BTGhostController {
protected:
	std::shared_ptr<Behavior> createChaseBehavior() override;
	std::shared_ptr<Behavior> createFrightenedBehavior() override;

public:
	PinkyController(std::shared_ptr<Character> character);
	virtual ~PinkyController();
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