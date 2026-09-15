#pragma once

#include "BTGhostController.h"


class InkyController: public BTGhostController {
	std::shared_ptr<Character> blinky;
protected:
	std::shared_ptr<Behavior> createChaseBehavior() override;
public:
	InkyController(std::shared_ptr<Character> character, std::shared_ptr<Character> blinky = nullptr);
	virtual ~InkyController();
};

class InkyChase : public Behavior{
	std::shared_ptr<Character> blinky;
public:
	InkyChase(std::shared_ptr<Character> blinky);
	virtual Status update() override;
};
