Khazad
======

A Strategy game based on Dwarf Fortress, if this game were a song it would be https://www.youtube.com/watch?v=ytWz0qVvBZ0

Khazad aims to become the preminent opensource Dwarf Fortress like game by leveraging the talents and skills of the genres small but highly skilled fan base.

A firm technical foundation is being built first, so far this includes

*  A flexible map structure structure that can be expanded in any direction at runtime and which tracks volumes as well as surfaces.
*  Highly optimized and multithreaded pathfinding which renders everything else in the genre obsolete, unit counts of ~1000 should be possible.
*  3D rendering of terrain, rotating camera and visual slicing of the map to overcome confusion inherent in isometric or 2D perspectives.
*  Efficient object update queue, rather then iterating every game object every loop all objects are awoken at a dynamic determined future time.

Still under construction...

*  A new Job managment system which groups individual tasks into logical group and assigns workers the same task type repetedly to reduced wasted travel time.
*  Demand-pull basied crafting and stockpile managment also know as 'Kanban', consumption triggers production without micromanagment


GAMEPLAY GOALS

Khazad aims to be Deep, Challenging and Accessible.

Deepth comes from a complex and emergent interaction between the choices the player is presented with.  Choices need to be impactfull to the game and non-trivial for the player to find the best solution.  The classic example is Chess, the player has a modest set of choices, but they are very impactfull and very difficult to choose between.  In creating a settlment the player should be faced with numerous choices of what and in what order to develop and how to manage conflicting internal interest groups.  Their should be no EASY choices or choices without a downside.  All elements within the game world which are presented and uniquely named will be differentiated in some gameplay manor, meer 'cosmetic' complexity will be shunned.

Khazad aims to be Challenging, it has become the norm to think of the DF-like genre as a 'sandbox' inwhich the player (once familiar with the game) builds a settlment that is very resilent and will generally resist all external abuse given even a modicum of effort by the player. In contrast Khazad will be intended as a challenge for the player at all times, a game in which survival of the settlment is always in doubt and being knocked-back several stages is common occourance.  Should the player build their settlment to a mighty city they will face the challenges of keeping and maintaining a kingdom built on fragil alliances, diplomacy and the sword.

Lastly Accesible, the deficiencies of Dwarf Fortress UI have already been mostly corrected in games such as Gnomoria, Khazad aims to have a comperable UI built using Nifty.  3D rendering adds additional accesibility as was seen in "A game of Dwarves" and Khazad will feauture nearly identical earth X-ray like rendering of underground tunnels.


KEY CONCEPTS

Dwarf attributes, in keeping with an RPG theme all creatures have a basic set of attributes, but keeping with the detail orientation of the genre the attribute set is larger then the 6 in D&D, a set of 14 attributes will be used, 7 Physically oriented (Strength, Flexibility, Dextarity, Vitality, Reflexes, Endurace, Speed) and 7 Mental oriented (Logic, Perception, Charisma, Memory, Judgement, Willpower, Intuition).  A roll of 2D4 is the normal range for each attribute.  Each attribute will directly and transparently effect a different aspect of a creatures behavior and performance in certain jobs and skils.

Skills, rather then the genre standard of each Dwarf having a skill level in each possible skill Khazad will combine the class-tree stystem of 'Evil Genius' and 'A game of Dwarves' with the classes of Dwarf Fortress into a large tree of perhapse 3 tiers with an initial first tier division into 7 core classes (Worshiper, Thinker, Fighter, Crafter, Builder, Laborer, Gatherer), 3rd tier divisions may yield upwards of 100-200 highly specilized classes, but the bulk of gameplay will be done with the core classes and their 2nd tier divisions.  Low level classes act as generalists so a settlment can be effectivly run with just 7 Dwarves.  Classic D&D character classes will be present to fill a variety of roles and multi-classing might also be possible.

Off Map activity, rather then having the players settlment sit passivly and recive attacks, trade and migrants from an outside world the player in Khazad will send OUT just as many interactions as they recive.  These Expeditions consist of hand-picked dwarves and stockpiled materials that are directed to a particular world map location with a specific goal, all of which have an effect on the outcome.  The Expeditions are run and resolved entirly in the background without players being able to control or see the results until the expedition returns (if it returns).  Significant political activity, wars, alliances and the like will play out with the player bringing the whole of the Dwarven race under their kingship as a possible outcome.
