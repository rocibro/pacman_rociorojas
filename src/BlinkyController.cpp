#include "BlinkyController.h"


BlinkyController::BlinkyController(std::shared_ptr<Character> character):
	FSMController(character){
}

BlinkyController::~BlinkyController() {

}

