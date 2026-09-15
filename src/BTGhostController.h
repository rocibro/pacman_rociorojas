/*
 * BTGhostController.h
 *
 *  Created on: May 2, 2018
 *      Author: VIDEOJUEGOS UTALCA
 */

#ifndef BTGHOSTCONTROLLER_H_
#define BTGHOSTCONTROLLER_H_

#include "Controller.h"
#include "BehaviorTree.h"
#include <chrono>

#include <random>
class Info{
    static Info *info;
    Info(){}

public:
    static Info* getInfo(){
        if(info==nullptr)info = new Info();
        return info;
    }
    const GameState* in_gamestate;
    Move out_move;
    std::shared_ptr<Character> in_character;
};

class BTGhostController: public Controller {
protected:
    std::shared_ptr<Composite> root;
    
    //para que las subclases puedan sobreescribir lo que sea necesario
    virtual std::shared_ptr<Behavior> createChaseBehavior();
    virtual std::shared_ptr<Behavior> createScatterBehavior();
    virtual std::shared_ptr<Behavior> createFrightenedBehavior();
    virtual std::shared_ptr<Behavior> createPowerPillCondition();
    virtual std::shared_ptr<Behavior> createScatterCondition();

    void buildTree();
public:
	BTGhostController(std::shared_ptr<Character> character);
	virtual ~BTGhostController();
	virtual Move getMove(const GameState& gs) override;
};

class Chase : public Behavior{
public:
    virtual Status update() override;

};

class Frightened : public Behavior{
public:
    virtual Status update() override;
};

class Scatter : public Behavior{
private:
    std::pair<int,int> cornerTarget;

public:
    virtual Status update() override;
    Scatter();

};

class Powerpill : public Behavior{
public:
    virtual Status update() override;
};

class TimeOut : public Behavior{
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
public:
    virtual Status update() override;
    TimeOut ();
};

#endif /* BTGHOSTCONTROLLER_H_ */
