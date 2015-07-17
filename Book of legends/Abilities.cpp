#include "Battle_system.h"

// File contains the basics of ability system of "Book of legends"

class Effect;

/*template for Effect:
class :public Effect //			Description
{
public:
	(int duration, double value):Effect(duration, value) {positive = ();};

	void initialize_effect(Unit & caster, Unit & target)
	{
		
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		
	}
};*/

// List of effects on characters:

class HP_Affect:public Effect //				Instantly adds value to current HP or duration turns increases current HP by value
{
public:
	HP_Affect(int duration, double value):Effect(duration, value), HP_override(0) {positive = (value > 0);};

	void initialize_effect(Unit & caster, Unit & target)
	{
		HP_override += target.modify_HP(value);
		if(target.get_HP() <= 0)
			target.kill();
	}

	void apply_effect(Unit & caster, Unit & target)
	{
		HP_override += target.modify_HP(value);
		if(target.get_HP() <= 0)
			target.kill();
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		if(!target.is_dead())
		{
			target.modify_HP(HP_override);
			target.modify_HP((-1)*value);
		}
	}
private:
	short HP_override; //							Stores the amount of HP that was added over max_HP
};

class HP_Affect_temporary:public Effect //			Gives a temporary shield that absorbs damage, does nothing if value is non-positive
{
public:
	HP_Affect_temporary(int duration, double value):Effect(duration, value), HP_pool(0) {positive = true;};

	void initialize_effect(Unit & caster, Unit & target)
	{
		HP_override = target.modify_HP(value);
		HP_observations = target.get_HP();
	}

	void apply_ability(Unit & caster, Unit & target)
	{
		if(active)
		{
			if(target.get_HP() < HP_observations)
			{
				HP_pool += (HP_observations - target.get_HP());
				if(HP_pool <= value)
					target.modify_HP(HP_observations - target.get_HP());
				else
				{
					target.modify_HP(HP_observations - target.get_HP() - value + HP_pool);
					active = false;
					HP_pool = value;
				}
			}
			HP_observations = target.get_HP();
		}
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		target.modify_HP(HP_override);
		if(active)
			target.modify_HP(value - HP_pool);
	}

private:
	bool active; //						Shows whether shield have worked out
	short HP_observations; //			Stores last observed HP of a target
	short HP_pool; //					Stores the damage taken by shield
	short HP_override; //				Stores the amount of HP that was added over max_HP
};

class MP_Affect_temporary:public Effect //			Gives a temporary mana to use abilities, does nothing if value is non-positive
{
public:
	MP_Affect_temporary(int duration, double value):Effect(duration, value), MP_pool(0) {positive = true;};

	void initialize_effect(Unit & caster, Unit & target)
	{
		MP_override = target.modify_MP(value);
		MP_observations = target.get_MP();
	}

	void apply_ability(Unit & caster, Unit & target)
	{
		if(active)
		{
			if(target.get_MP() < MP_observations)
			{
				MP_pool += (MP_observations - target.get_MP());
				if(MP_pool <= value)
					target.modify_MP(MP_observations - target.get_MP());
				else
				{
					target.modify_MP(MP_observations - target.get_MP() - value + MP_pool);
					active = false;
					MP_pool = value;
				}
			}
			MP_observations = target.get_MP();
		}
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		target.modify_MP(MP_override);
		if(active)
			target.modify_MP(value - MP_pool);
	}

private:
	bool active; //						Shows whether all the mana was spent
	short MP_observations; //			Stores last observed MP of a target
	short MP_pool; //					Stores the amount of mana that was used
	short MP_override; //				Stores the amount of MP that was added over max_MP
};

class MP_Affect:public Effect //				Instantly adds value to current MP or duration turns increases current MP by value
{
public:
	MP_Affect(int duration, double value):Effect(duration, value), MP_override(0) {positive = (value > 0);};

	void initialize_effect(Unit & caster, Unit & target)
	{
		MP_override += target.modify_MP(value);
	}

	void apply_effect(Unit & caster, Unit & target)
	{
		MP_override += target.modify_MP(value);
		if(target.get_MP() <= 0)
			target.kill();
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		target.modify_MP(MP_override);
		target.modify_MP((-1)*value);
	}
private:
	short MP_override; //						Stores the amount of MP that was added over max_MP
};

class AP_Affect:public Effect //			Adds value to AP and decreases AP after duration
{
public:
	AP_Affect(int duration, double value):Effect(duration, value) {positive = (value > 0);};

	void initialize_effect(Unit & caster, Unit & target)
	{
		if(!(*this).is_instant()) //		Duration != 0
		target.modify_AP(value);
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		target.modify_AP((-1)*value);
	}
};

class max_HP_Affect:public Effect //			Multiply max_HP with value and change current HP so that percentage of health be the same for duration
{
public:
	max_HP_Affect(int duration, double value):Effect(duration, value) 
	{
		if(duration == 0) // Must not be instant
			duration++;
		positive = (value > 1);
	};

	void initialize_effect(Unit & caster, Unit & target)
	{
		target.modify_max_HP(value);
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		target.modify_max_HP(1/value);
	}
};

class max_MP_Affect:public Effect //			Multiply max_MP with value and change current MP so that percentage of health be the same for duration
{
public:
	max_MP_Affect(int duration, double value):Effect(duration, value) 
	{
		if(duration == 0) // Must not be instant
			duration++;
		positive = (value > 1);
	};

	void initialize_effect(Unit & caster, Unit & target)
	{
		target.modify_max_MP(value);
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		target.modify_max_MP(1/value);
	}
};

class Hit_Chance_Affect:public Effect //			Multiplies hit chance of target by value for duration
{
public:
	Hit_Chance_Affect (int duration, double value):Effect(duration, value) 
	{		
		if(duration == 0) // Must not be instant
			duration++;
		positive = (value > 1);
	};

	void initialize_effect(Unit & caster, Unit & target)
	{
		if(!(*this).is_instant()) //		Duration != 0
		target.modify_hit_chance(value);
	}

		void remove_effect(Unit & caster, Unit & target)
	{
		target.modify_hit_chance(1/value);
	}
};

class Dodge_Chance_Affect:public Effect //			Multiplies dodge chance of target by value for duration
{
public:
	Dodge_Chance_Affect (int duration, double value):Effect(duration, value) 
	{
		if(duration == 0) // Must not be instant
			duration++;
		positive = (value > 1);
	};

	void initialize_effect(Unit & caster, Unit & target)
	{
		if(!(*this).is_instant()) //		Duration != 0
		target.modify_dodge_chance(value);
	}

		void remove_effect(Unit & caster, Unit & target)
	{
		target.modify_dodge_chance(1/value);
	}
};

class Covered_Affect:public Effect //			Multiplies field "covered" of a target by value for a duration
{
public:
	Covered_Affect(int duration, double value):Effect(duration, value) {positive = (value > 1); };

	void initialize_effect(Unit & caster, Unit & target)
	{
		target.modify_covered(value);
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		target.modify_covered(1/value);
	}
};

class Current_Initiative_Affect:public Effect //			Adds value to current initiative of a unit. Sets everything back after duration
{
public:
	Current_Initiative_Affect(int duration, double value):Effect(duration, value) {positive = (value > 0);};

	void initialize_effect(Unit & caster, Unit & target)
	{
		target.modify_current_initiative(value);
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		target.modify_current_initiative((-1)*value);
	}
};

class Initiative_Affect:public Effect //			Adds value to initiative for duration, initiative can not be modified to be less than 1 or more then INITIATIVE_CONSTANT
{
public:
	Initiative_Affect(int duration, double value):Effect(duration, value), initiative_override(0) 
	{		
		if(duration == 0) // Must not be instant
			duration++;
		positive = (value > 0);
	};

	void initialize_effect(Unit & caster, Unit & target)
	{
		initiative_override = target.modify_initiative(value);
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		target.modify_initiative(initiative_override);
		target.modify_initiative((-1)*value);
	}
private:
	char initiative_override;
};

class Remove_Buff_Affect:public Effect //			Removes all buffs from target if value == 0, removes all debuffs otherwise. If not instant, sets them back if they are not expired
{
public:
	Remove_Buff_Affect(int duration, double value):Effect(duration, value) {positive = (value != 0);};

	void initialize_effect(Unit & caster, Unit & target) // Take away all buffs/debuffs
	{
		for(auto iter = target.applied_abilities.begin(); iter != target.applied_abilities.end(); ++iter)
			if(value)
			{
				if((*iter)->is_debuff()) // If is a debuff
				{
					taken_buffs.push_back(*iter); //		Store it in taken_buffs
					(*iter)->remove_ability(target); //		Apply remove_ability
					target.applied_abilities.erase(iter); //Remove from applied_abilities
				}
			}
			else
			{
				if((*iter)->is_buff()) // If is a buff/debuff
				{
					taken_buffs.push_back(*iter); //		Store it in taken_buffs
					(*iter)->remove_ability(target); //		Apply remove_ability
					target.applied_abilities.erase(iter); //Remove from applied_abilities
				};
			}
	}

	void apply_effect(Unit & caster, Unit & target) //	Ensure that their clock is still ticking
	{
		for(auto iter = taken_buffs.begin(); iter != taken_buffs.end(); ++iter)
		{
			(*(*iter))++;
			if((*iter)->expired())
				taken_buffs.erase(iter);
		}
	}

	void remove_effect(Unit & caster, Unit & target) //	Return unexpired ones
	{
		for(auto iter = taken_buffs.begin(); iter != taken_buffs.end(); ++iter)
			(*(*iter)).initialize_ability((*(*iter)).get_ability_caster(),target,(*(*iter)).get_duration_counter());
	}
	vector <Ability_Active *> taken_buffs;
};