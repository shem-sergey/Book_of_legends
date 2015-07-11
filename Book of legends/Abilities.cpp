#include "Battle_system.h"

// File contains the basics of ability system of "Book of legends"

class Effect;
class Effect_On_Character;
class Effect_On_Item;

class Effect
{
public:
	Effect(int duration):duration(duration) {};
	bool expired(int turn)
	{
		return turn > duration;
	}
	bool is_instant()
	{
		return duration == 0;
	}
	char duration; //					Duration of the effect
};

class Effect_On_Character: public Effect
{
public:
	Effect_On_Character(int duration, bool operation, double value):Effect(duration), operation(operation), value(value) {};
	bool operation; //					true = "+", false = "*"
	double value; //					modifier
};

// List of effects on characters:

class HP_Affect:public Effect_On_Character
{
public:
	HP_Affect(int duration, bool operation, double value):Effect_On_Character(duration, operation, value) {};

	void apply_effect(Unit & target)
	{
		HP_override = target.modify_HP(operation, value);
		if(target.get_HP() <= 0)
			target.kill();
	}

	void remove_effect(Unit & target)
	{
		if(!target.is_dead())
		{
			target.modify_HP(true, HP_override);
			if(operation)
				target.modify_HP(operation, (-1)*value);
			else
				target.modify_HP(operation, 1/value);
		}
	}
private:
	int HP_override;
};
class MP_Affect:public Effect_On_Character
{
public:
	MP_Affect(int duration, bool operation, double value):Effect_On_Character(duration, operation, value) {};

	void apply_effect(Unit & target)
	{
		target.modify_MP(operation, value);
	}

	void remove_effect(Unit & target)
	{
		target.modify_MP(true, MP_override);
		if(operation)
			target.modify_MP(operation, (-1)*value);
		else
			target.modify_MP(operation, 1/value);
	}
private:
	int MP_override;
};
class AP_Affect:public Effect_On_Character
{
public:
	AP_Affect(int duration, bool operation, double value):Effect_On_Character(duration, operation, value) {};

	void apply_effect(Unit & target)
	{
		target.modify_AP(operation, value);
	}

	void remove_effect(Unit & target)
	{
			if(operation)
				target.modify_AP(operation, (-1)*value);
			else
				target.modify_AP(operation, 1/value);
	}
};