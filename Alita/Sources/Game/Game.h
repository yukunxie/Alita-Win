//
// Created by realxie on 2019-11-03.
//

#ifndef ALITA_GAME_H
#define ALITA_GAME_H

#include "../Engine/Engine.h"
#include "../World/Level.h"
#include "../World/World.h"

/*
 * This is the entry of a game
 * */
class GamePlay
{
public:
    GamePlay();
    ~GamePlay();

protected:
    void InitWorld();

protected:
    NS_RX::Engine* gameEngine_;
    NS_RX::World*  gameWorld_;
};

#endif //ALITA_GAME_H
