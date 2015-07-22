#include "Battle_system.h"

//Class Unit methods begin here:

bool Unit::is_dead(){ return dead; }
int Unit::get_current_initiative() {  return current_initiative;  }
int Unit::get_HP() { return HP; }
int Unit::get_MP() { return MP; }
int Unit::get_AP() { return AP; }
int Unit::get_hit_chance() { return hit_chance; }
int Unit::get_dodge_chance() { return dodge_chance; }
int Unit::get_toughness() { return toughness; }
int Unit::get_weapon_skill(int type) { return weapon_skill[type]; }
double Unit::get_covered() { return covered; };

int Unit::modify_HP(double value)
{
		HP += value;
	if(HP > max_HP)
	{
		int result = HP - max_HP;
		HP = max_HP;
		return result;
	}
	if(HP < 0)
	{
		int result = 0 - HP;
		HP = 0;
		return result;
	}
	return 0;
}
int Unit::modify_MP(double value)
{
		MP += value;
	if(MP > max_MP)
	{
		int result = MP - max_MP;
		MP = max_MP;
		return result;
	}
	if(MP < 0)
	{
		int result = MP;
		MP = 0;
		return result;
	}
	return 0;
}
void Unit::modify_AP(double value)
{
		AP += value;
}
void Unit::modify_max_HP(double value)
{
	max_HP *= value;
	HP *= value;
}
void Unit::modify_max_MP(double value)
{
	max_MP *= value;
	MP *= value;
}
void Unit::kill()
{
	dead = true;
}
void Unit::revive()
{
	dead = false;
}
void Unit::modify_current_initiative(double value)
{
	current_initiative += value;
}
int Unit::modify_initiative(double value) // Initiative can not be less then 1 or more than INITIATIVE_CONSTANT
{
	int result = 0;
	initiative += value;
	if(initiative < 1)
	{
		result = initiative - 1;
		initiative = 1;
	}
	if(initiative > INITIATIVE_CONSTANT)
	{
		result = initiative - INITIATIVE_CONSTANT;
		initiative = INITIATIVE_CONSTANT;
	}
	return result;
}
void Unit::modify_hit_chance(double value)
{
	hit_chance *= value;
}
void Unit::modify_dodge_chance(double value)
{
	dodge_chance *= value;
}
void Unit::modify_covered(double value)
{
	covered *= value;
}

void Unit::show()
{
	cout << name << endl << background << endl;
}

Ability_Active & Unit::choose_ability(Battle & enviroment)
{
	Ability_Active blank;
	return blank;
}

Unit & Unit::choose_target_for_ability(Battle & enviroment, Ability_Active & chosen_abilty)
{
	Unit blank;
	return blank;
}

//Class Unit methods end here.

//Class Hero methods begin here:

void Hero::show()
{
	cout << "Hero ";
	(*this).Unit::show();
}

//Class Hero methods end here.

//Class Weapon methods begin here:

int Weapon::get_hit_modifier(){ return to_hit_modifier; }
int Weapon::get_type(){ return type; }
int Weapon::get_damage(){ return damage; }
double Weapon::get_AP_modifier(){ return AP_modifier;};

//Class Weapon methods end here.

//Class Ability_Active methods begin here:

int Ability_Active::get_actioncost()
{
	return actioncost;
}
int Ability_Active::get_manacost()
{
	return manacost;
}
int Ability_Active::get_duration_counter()
{
	return duration_counter;
}
int Ability_Active::get_return_value()
{
	return return_value;
}
Unit & Ability_Active::get_ability_caster()
{
	return *ability_caster;
}

bool Ability_Active::is_instant()
{
	bool result = 1;
	for(auto iter = effects.begin(); iter != effects.end(); ++iter)
		result *= iter->is_instant();
	return result;
}
bool Ability_Active::is_buff()
{
	for(auto iter = effects.begin(); iter != effects.end(); ++iter)
		if(iter->is_buff())
			return true;
	return false;
}
bool Ability_Active::is_debuff()
{
	for(auto iter = effects.begin(); iter != effects.end(); ++iter)
		if(!iter->is_buff())
			return true;
	return false;
}
bool Ability_Active::expired()
{
	for(auto iter = effects.begin(); iter != effects.end(); ++iter)
		if(!iter->expired(duration_counter))
			return false;
	return true;
}

void Ability_Active::operator++()
{
	duration_counter++;
}

int Ability_Active::initialize_ability(Unit & caster, Unit & target, int turn)
{
	for(auto iter = effects.begin(); iter != effects.end(); ++iter)
		iter->initialize_effect(caster, target);
	duration_counter = turn;
	*ability_caster = caster;
	return return_value;
}

void Ability_Active::apply_ability(Unit & target)
{
	for(auto iter = effects.begin(); iter != effects.end(); ++iter)
		if(iter->expired(duration_counter))
			iter->apply_effect(*ability_caster, target);
	++duration_counter;
}

void Ability_Active::remove_ability(Unit & target)
{
	for(auto iter = effects.begin(); iter != effects.end(); ++iter)
		iter->remove_effect(*ability_caster, target);
}

//Class Ability_Active methods end here.

//Class Attack methods begin here.

Attack::Attack():Ability_Active(1){}

int Attack::initialize_ability(Unit & caster, Unit & target, int turn)
{
	// Calculating apcost of this attack:
	int AP_cost = (*this).get_actioncost() * caster.weapon.get_AP_modifier() * target.get_covered();
	if(caster.get_AP() >= AP_cost) // Check whether attack can be really applied
	{
		caster.modify_AP((-1)*(AP_cost - (*this).get_actioncost())); // Substract difference between actioncost and real cost of attack
		// Calculating percent of successful allack:
		int attack_chance = caster.get_hit_chance() + (caster.weapon.get_hit_modifier()*caster.get_weapon_skill(caster.weapon.get_type()))/100;
		attack_chance -= target.get_dodge_chance();
		int dice = rand() % 100; //				Rolling d100
		if(dice < attack_chance) //				If roll is successful:
		{
			// Calculating damage:
			int damage = caster.weapon.get_damage()*(caster.get_weapon_skill(caster.weapon.get_type())+100)/200;
			damage -= target.get_toughness();
			target.modify_HP((-1) * damage); // Applying damage
		}
		Ability_Active::initialize_ability(caster, target, turn);
	}
	return 1;
}

//Class Attack methods end here:

//Class Battle methods begin here:

Battle::Battle()
{
	srand(time(NULL)); //		Seed for random generation in this battle
}

int Battle::process_situation()
{
	// Checking whether party is dead:
	auto iterator1 = find_if((party_in_battle->creatures).begin(), (party_in_battle->creatures).end(), [](Unit x){ return x.is_dead() == false;});
	if(iterator1 == (party_in_battle->creatures).end())
		return 0;

	// Checking whether enemies are dead:
	auto iterator2 = find_if((opponents_in_battle->creatures).begin(), (opponents_in_battle->creatures).end(), [](Unit x){ return x.is_dead() == false;});
	if(iterator2 == (opponents_in_battle->creatures).end())
		return 2;

	// Still fighting:
	while(battle_queue.empty())
	{
		// Increasing current initiative for every unit:
		for_each((party_in_battle->creatures).begin(), (party_in_battle->creatures).end(), [](Unit x){ x.modify_current_initiative(x.initiative); });
		for_each((opponents_in_battle->creatures).begin(), (opponents_in_battle->creatures).end(), [](Unit x){ x.modify_current_initiative(x.initiative); });
		
		vector <Hero> temp_heroes; //		List of heroes that are ready to act in combat
		vector <Unit> temp_enemies; //		List of enemies that are ready to act in combat

		// Making lists of units that are ready to act in combat:
		copy_if((party_in_battle->creatures).begin(), (party_in_battle->creatures).end(), temp_heroes.begin(), 
			[](Unit x)
			{
				if(x.get_current_initiative() >= INITIATIVE_CONSTANT)
				{
					x.modify_current_initiative((-1)*x.initiative);
					return true;
				}
				return false;
			});
		copy_if((opponents_in_battle->creatures).begin(), (opponents_in_battle->creatures).end(), temp_enemies.begin(), 
			[](Unit x)
			{
				if(x.get_current_initiative() >= INITIATIVE_CONSTANT)
				{
					x.modify_current_initiative((-1)*x.initiative);
					return true;
				}
				return false;
			});

		// Sorting lists of units that are ready to act in combat:
		sort(temp_heroes.begin(), temp_heroes.end(), [](Unit x, Unit y){return x.get_current_initiative() < y.get_current_initiative();});
		sort(temp_enemies.begin(), temp_enemies.end(), [](Unit x, Unit y){return x.get_current_initiative() < y.get_current_initiative();});

		auto iterator3 = temp_heroes.begin();
		auto iterator4 = temp_enemies.begin();

		// Merging two sorted lists into one battle queue:
		while(iterator3 != temp_heroes.end() || iterator4 != temp_enemies.end())
		{
			if(iterator3->get_current_initiative() > iterator4->get_current_initiative())
			{
				battle_queue.push(&(*iterator3));
				++iterator3;
			}
			else
			{
				battle_queue.push(&(*iterator4));
				++iterator4;
			}
		}

		if(iterator3 == temp_heroes.end())
			while(iterator4 != temp_enemies.end())
			{
				battle_queue.push(&(*iterator4));
				++iterator4;
			}
		else
			while(iterator3 != temp_heroes.end())
			{
				battle_queue.push(&(*iterator3));
				++iterator3;
			}
	}

	return 1;
}

int Battle::process_unit()
{
	int special_situation = 0; //								Used to process special situations, return value of initialize_ability. Zero by default

	Unit &active_unit = *(battle_queue.front()); //				Unit that will act in this turn
	battle_queue.pop(); //										Popping battle queue

	for(auto iterator = active_unit.applied_abilities.begin(); iterator != active_unit.applied_abilities.end(); ++iterator)
	{
		iterator->apply_ability(active_unit); //			Applying all the abilities
		if(iterator->expired()) //							And if some ability was expired
		{
			iterator->remove_ability(active_unit);	//		Applying remove_ability and
			active_unit.applied_abilities.erase(iterator); //	Removing it from list of applied abilities
		}
	}

	int AP_buffer = active_unit.get_AP(); //			Saving AP of a unit	

	if(!active_unit.is_dead()) //						Check if active unit is already dead
		while(active_unit.get_AP() > 0) //				Continie while active unit can act
		{
			Ability_Active chosen_ability = active_unit.choose_ability(*this); //	Choose ability
			Unit &ability_target = active_unit.choose_target_for_ability(*this, chosen_ability); //Choose target
			if(chosen_ability.get_actioncost() <= active_unit.get_AP() && //	Check whether ability is usable
			   chosen_ability.get_manacost() <= active_unit.get_MP())
			{
				active_unit.modify_AP((-1)*chosen_ability.get_actioncost()); //							Pay for ability
				active_unit.modify_MP((-1)*chosen_ability.get_manacost());

				special_situation = chosen_ability.initialize_ability(active_unit, ability_target); //	Initialize ability

				if(!chosen_ability.is_instant())
					ability_target.applied_abilities.push_back(chosen_ability); //						If not instant, place ability to applied abilities of a target
			}
			// Here are all checks for special situations
		}

		active_unit.modify_AP((-1)*active_unit.get_AP()); //	Ensuring AP of a unit is equal to 0
		active_unit.modify_AP(AP_buffer); //					Restoring AP of a unit

		return special_situation;
}

//Class Battle methods end here.