#ifndef BATTLE_SYSTEM_OF_BOOK_OF_LEGENDS
#define BATTLE_SYSTEM_OF_BOOK_OF_LEGENDS

#include <iostream>
#include <cstdlib>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

//Constants

const int INITIATIVE_CONSTANT = 10;

//Firstly basic classes for storing data:

class Ability;
class Item;
class Consumable_Item;
class Weapon;
class Artifact;
class Armor;
class Unit;
class Ability_Passive;
class Ability_Active;
class Ability_Active_On_Target;
class Ability_Active_On_Group;
class Hero;
class Party;
class Opponents;

//Secondly classes for implementing battle

class Battle;

class Ability
{
public:
	// Some information for the atmosphere
	string name;
	string background;
private:
	int value; //									It's for the game balance, I'm serious!
};

//Important!: In "Item" class there is a field "heroes" that has to be modifyed with the adding of new heroes!

class Item
{
public:
	// Some information for the atmosphere
	string name;
	string background;
private:
	int value; //									It's for the game balance, I'm serious!

	int cost; //									Cost of item in gold pieces
	vector <char> heroes; //						List of heroes that can use this item

	vector <Ability*> abilities; //					List of abilities of an item
};

class Consumable_Item: public Item
{
public:
private:
	char capacity; //								Amount of items that can be placed in one stot
};

class Weapon: public Item
{
public:
private:
	char to_hit_modifier; //						Displays the inprovement of to-hit chance while hitting with this weapon
	char damage; //									Damage, done by attack with this weapon
	double AP_modifyier; //							Modifyer of action points needed to hit with this weapon
	bool twohanded; //								Displays whether the second hand can be used
};

class Artifact: public Item
{
public:
private:
	Ability_Passive *buff; //						Passive ability of the artifact
};

class Armor: public Item
{
public:
private:
	Ability_Passive *protection; //					Passive ability of protecting the wearer
};

class Unit
{
public:
	// Some information for the atmosphere
	string name;
	string background;

	// Public stats of the unit:

	short max_HP;//									Maximal hit points
	short max_MP;//									Maximal mana points
	short max_AP;//									Maximal action points
	char initiative; //								Initiative of a unit
	int value; //									It's for the game balance, I'm serious!

	bool is_dead();
	void modify_current_initiative(int); //			Adds x to current initiative of unit
	int get_current_initiative();

private:
	//Stats of a unit:

	short HP; //									Current hit points
	short MP;//										Current mana points
	short AP;//										Current action points

	char current_initiative; //						Current initiative in battle for queueing
	char hit_chance; //								Chance of a unit to hit, 0<hit_chance<100
	char dodge_chance; //							Chance of a unit to dodge, 0<dodge_chance<100
	double covered; //								AP modifier to attack this unit
		
	bool dead; //									Shows whether unit is dead in particular combat

	//Use of other classes:

	vector <Ability*> abilities; //					List of abilities of a unit
	vector <Item> arms; //							Items in the unit's hands
	Armor armor; //									Armor unit is wearing
	vector <Artifact> artifacts; //					List of artifacts unit is carryings
	vector <Consumable_Item> consumables; //		List of consumable items
};

class Ability_Passive: public Ability
{
public:
	void obtaining_ability(Unit*){}; //				Method, applied when obtainig abiity
	void discarding_ability(Unit*){}; //			Method, applied when discarding ability
private:
};

class Ability_Active: public Ability
{
public:
private:
	short manacost; //								Amount of mana needed to use ability
	short actioncost; //							Amount of action points needed to use ability
};

class Ability_Active_On_Target: public Ability_Active
{
public:
	void apply_ability(Unit*){}; //					Method used to apply the ability
private:
};

class Ability_Active_On_Group: public Ability_Active
{
public:
	template <typename T>
	void apply_ability(T*){}; //					Method used to apply the ability
private:
};

class Hero: public Unit
{
public:
private:
	// Specific PC attributes
	char alignment; //								Reflects alignment of a character
	char level; //									Level of a character
	int experience; //								Amount of XP gained
	char influence; //								Reflects the influence of the character on others

	vector <Item> loot; //							Stores hordes of loot
};

class Party
{
public:
	vector <Hero> creatures;//						List of heroes in the party
private:	
	double alignment; //							Average alignment of a party
	int gold; //									Treasures of a party

	int value; //									It's for the game balance, I'm serious!
};

class Opponents
{
public:
	vector <Unit> creatures;//						List of enemies in the group
private:

	int gold; //									Amount of gold reward
	int experience; //								Amount of experience reward
	vector <Item> loot; //							Stores hordes of loot reward

	int value; //									It's for the game balance, I'm serious!
};

class Battle
{
public:
	int process_situation(); //						Check whether someone is dead and add units to battle queue if there are none
							 //						Returns 0 if party is dead, 2 if party wins, 1 if none of this
private:
	Party *party_in_battle; //						Pointer to a party of adventurers
	Opponents *opponents_in_battle; //				Pointer to a party of enemies
	queue <Unit*> battle_queue; //					Queue of acting units
	vector <Ability_Active> *current_abilities;//	Abilities of a selected unit	
};

#endif 