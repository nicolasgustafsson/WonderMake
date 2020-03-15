Buffs are limited scope modifications of characters. They can be applied through weapons, enemies and world objects(traps, random objects)

These are some examples of what the buff system should be able to do minimally (procedurally):

* A buff that deals damage every 3 seconds for 10 seconds
* A buff that increases the movespeed of the character while in a certain radius

These are some examples of what a buff system **ideally** should be able to do (procedurally):

* A buff that heals the player 3 times upon taking lethal damage
* A buff that increases damage every time the player hits an enemy but is lost after some time
* A buff that kills the owner after taking any kind of damage
* A buff that binds two characters together, making them share a health pool
* A buff that reserves a resource for a character in exchange for a bonus in attack damage


Use modules to build behavior:
	One module to tick every x dealing damage
	One module to kill the buff after x seconds

Designer is responsible for creating modules that do not interfere with eachother(somehow)

Buffs have 2 parts. One blueprint part, and one instanced part:
* The blueprint part contains the information for the buff type
* The instanced part contains the information stored per buff instance

For example, a weapon might apply a buff on hit. The weapon then has the buff blueprint create an instance on the character when hitting them.


BuffBlueprint     | LifetimeBuffProperty
BuffInstance	  | TimeLeft