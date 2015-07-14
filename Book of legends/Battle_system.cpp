#include "Battle_system.h"

//Class Unit methods begin here:

bool Unit::is_dead(){ return dead; }
int Unit::get_current_initiative() {  return current_initiative;  }
int Unit::get_HP() { return HP; }
int Unit::get_MP() { return MP; }
int Unit::get_AP() { return AP; }

int Unit::modify_HP(double value)
{
		HP += value;
	if(HP > max_HP)
	{
		int result = HP - max_HP;
		HP = max_HP;
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
void Unit::modify_hit_chance(double value)
{
	hit_chance *= value;
}
void Unit::modify_dodge_chance(double value)
{
	dodge_chance *= value;
}

void Unit::show()
{
	cout << name << endl << background << endl;
}

//Class Unit methods end here.

//Class Hero methods begin here:

void Hero::show()
{
	cout << "Hero ";
	(*this).Unit::show();
}

//Class Hero methods end here.

//Class Battle methods begin here:

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

//Class Battle methods end here.