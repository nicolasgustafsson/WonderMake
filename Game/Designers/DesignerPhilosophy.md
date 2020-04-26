The underlying idea behind designers is to emulate the human thought process; how would a live human being develop this thing in a gamedev environment?

Usually the process will be some varying of the following:

*Get handed a basic design or a list of requirements
*Develop an abstract idea of what it will do
*Concretize the abstract idea until the abstract idea has become the thing requested
*Hand the finished object to the requester

The goal of this is to generate content similar to how a studio would do it, for instance:

1. The director says we need a spooky level, it asks the level designer for one
2. The level designer creates a base level design, then asks environment art to make it spooky
3. Environment art adds a bunch of skulls to the level and hands it back to the level designer
4. The level designer asks weapon design to create a spooky weapon to go with the level
5. The weapon designer creates a spooky weapon and hands it to the level designer
6. The level designer returns it to the director which puts it in the game

Requirements VS Design

Some designers have a Requirements and a Design struct allocated to them. 
They often have very similar contents, but to preserve conceptual integrity they should remain separated.

Requirements are what the *requester* wants from the object. It can be things like "Make the enemy easy to kill", "Make the buff that will be applied on hit be positive"
and "Make the level be long".
Design is what the designer is working on before completing the object. These are often based on the requirements and then fleshed out. It is up to the designer how to 
implement the requirements: an enemy that is easy to kill may be something with very little health, or something that has a lot of health but is very slow.