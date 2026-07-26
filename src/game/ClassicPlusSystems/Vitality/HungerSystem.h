/*

*/

#ifndef HUNGER_SYSTEM_H
#define HUNGER_SYSTEM_H

class Player; // Forward declaration of the Player class, instead of including the entire Player.h header file. Makes the code more efficient and reduces dependencies.

class HungerSystem
{
public:
    //HungerSystem(); // Constructor for the HungerSystem class, runs once on server startup.
    //~HungerSystem(); // Destructor for the HungerSystem class, runs once on server shutdown.
    void Update(Player* player, uint32 diff); // called every server update. diff is milliseconds since last update (normally 50ms or 100ms).
    //void Update(Player*, uint32); // called every server update. diff is milliseconds since last update (normally 50ms or 100ms).

private:

    //uint32 m_updateTimer; // count variable
};

#endif