#pragma once

#include "FSMController.h"


class BlinkyController: public FSMController {

public:
	BlinkyController(std::shared_ptr<Character> character);
	virtual ~BlinkyController();
};

