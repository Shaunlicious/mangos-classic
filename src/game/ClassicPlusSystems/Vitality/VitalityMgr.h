/*

*/

#ifndef Vitality_Mgr_H
#define Vitality_Mgr_H

#include "HungerSystem.h"; // forward declaration of HungerSystem class

class Player; // forward declaration of Player class
//class HungerSystem; // forward declaration of HungerSystem class

class VitalityMgr
{
public:
    void Update(Player* player, uint32 diff);
private:
    HungerSystem m_hunger; // instance of HungerSystem to manage hunger updates
}; 

#endif