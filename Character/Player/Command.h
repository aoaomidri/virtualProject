#pragma once
#include"Player.h"
class Player;

class ICommand{
	virtual ~ICommand();
	virtual void Exec(Player& player) = 0;

};

class MoveRightCommand : public ICommand{
public:
	void Exec(Player& player)override;

};

class MoveLeftCommand : public ICommand {
public:
	void Exec(Player& player)override;

};


