/*
 * FSMController.h
 *
 *  Created on: Apr 23, 2018
 *      Author: nbarriga
 */

#ifndef FSMCONTROLLER_H_
#define FSMCONTROLLER_H_

#include "Controller.h"
#include <random>
#include "FSM.h"
#include <chrono>

class GhostStateMachine;

class FSMController: public Controller {
protected:
	std::shared_ptr<GhostStateMachine> fsm;
	virtual std::shared_ptr<FSMState> createChaseState();
	virtual std::shared_ptr<FSMState> createScatterState();
	virtual std::shared_ptr<FSMState> createFrightenedState();
	void buildFSM();
public:
	FSMController(std::shared_ptr<Character> character);
	virtual ~FSMController();
	virtual Move getMove(const GameState& game)override;
};

class PillTransition:public FSMTransition{
	int last;
	std::shared_ptr<FSMState> _next;
public:
	PillTransition(std::shared_ptr<FSMState> next);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;
};

class ChaseState:public FSMState{

public:
	ChaseState(std::shared_ptr<Character> _character);
	Move onUpdate(const GameState& gs) override;
	void onEnter(const GameState& gs) override;
	~ChaseState();

};

class ScatterState: public FSMState{
public:
	ScatterState(std::shared_ptr<Character> _character);
	void onEnter(const GameState&gs) override;
	Move onUpdate(const GameState& gs) override;
	~ScatterState();
};

class NonFrightenedState: public FSMState{
	std::shared_ptr<FSMState> chaseState;
	std::shared_ptr<FSMState> scatterState;
	std::shared_ptr<FSMState> activeChild; //cual de los dos está activo ahora

	std::chrono::system_clock::time_point lastSwitch; //cuando cambiamos por última vez
	bool started;

	static const int scatter_seconds = 7;
	static const int chase_seconds = 20;

public:
	NonFrightenedState(std::shared_ptr<Character> _character,
			std::shared_ptr<FSMState> chaseState,
			std::shared_ptr<FSMState> scatterState);
	void onEnter(const GameState& gs) override;
	Move onUpdate(const GameState& gs) override;
	void onExit(const GameState& gs) override;
	~NonFrightenedState();
};

class FrightenedState: public FSMState{
public:
	FrightenedState(std::shared_ptr<Character> _character);
	void onEnter(const GameState& gs) override;
	Move onUpdate(const GameState& gs) override;
	~FrightenedState();
};

class EdibleTransition: public FSMTransition{
	std::shared_ptr<Character> character;
	std::shared_ptr<FSMState> next;

public:
	EdibleTransition(std::shared_ptr<Character> _character, std::shared_ptr<FSMState> _next);
	bool isValid(const GameState& gs) override;
	std::shared_ptr<FSMState> getNextState() override;
};

class NotEdibleTransition: public FSMTransition{
	std::shared_ptr<Character> character;
	std::shared_ptr<FSMState> next;
public:
	NotEdibleTransition(std::shared_ptr<Character> _character, std::shared_ptr<FSMState> _next);
	bool isValid(const GameState& gs) override;
	std::shared_ptr<FSMState> getNextState() override;

};
class GhostStateMachine: public FiniteStateMachine{

public:
	GhostStateMachine(std::shared_ptr<Character> _character,
			std::shared_ptr<FSMState> chaseState,
			std::shared_ptr<FSMState> scatterState,
			std::shared_ptr<FSMState> frightenedState);
	Move update(const GameState& gs) override;
	~GhostStateMachine();

};
#endif /* FSMCONTROLLER_H_ */
