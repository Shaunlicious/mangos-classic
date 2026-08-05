// EatDrinkTracker.h
//
// Tracks start/duration/end of Food (SPELL_AURA_MOD_REGEN) and
// Drink (SPELL_AURA_MOD_POWER_REGEN) auras, and classifies the source
// of the consumable (conjured vs. regular vs. no-item/"other").
//
// Wire-up: call EatDrinkTracker::Instance().OnFoodAuraToggled(...) /
// OnDrinkAuraToggled(...) from Aura::HandleModRegen / HandleModPowerRegen
// in SpellAuras.cpp (see accompanying patch notes).

#ifndef MANGOS_EAT_DRINK_TRACKER_H
#define MANGOS_EAT_DRINK_TRACKER_H

//#include "Entities/ObjectGuid.h" // claude, ObjectGuid.h doesn't exist
//#include "Globals/ObjectMgr.h"
//#include <unordered_map>
//#include <ctime>

class Aura;
class Unit; // Claude forward declaration to avoid including Unit.h
class Player;
class Item;

enum class EatDrinkKind : uint8
{
    Food,
    Drink
};

enum class EatDrinkSource : uint8
{
    Conjured,   // e.g. Mage "Conjure Food"/"Conjure Water" items
    Regular,    // any other item-based food/drink (cooked, vendor, quest reward, etc.)
    Other       // aura applied without a backing item (NPC-granted, scripted, etc.)
};

struct EatDrinkSession
{
    EatDrinkKind   kind = EatDrinkKind::Food;
    EatDrinkSource source = EatDrinkSource::Other;
    uint32         spellId = 0;
    uint32         itemEntry = 0;   // 0 if no cast item
    time_t         startTime = 0;
    uint32         maxDurationMs = 0; // aura's own duration at the moment it started
};

class EatDrinkTracker
{
    public:
        static EatDrinkTracker& Instance();

        // Call from Aura::HandleModRegen(bool apply, bool Real)
        void OnFoodAuraToggled(Aura* aura, bool apply);

        // Call from Aura::HandleModPowerRegen(bool apply, bool Real)
        // (only call this when Real == true, matching the original handler's guard)
        void OnDrinkAuraToggled(Aura* aura, bool apply);

        void RegisterPendingItem(ObjectGuid guid, uint32 itemEntry);

        uint32 ConsumePendingItem(ObjectGuid guid);

    protected:
        // Override these two (or replace the sLog calls) to route events
        // to wherever you actually want them: a log, a DB table, an
        // achievement/statistics system, telemetry, etc.
        virtual void OnStart(Unit* target, EatDrinkSession const& session);
        virtual void OnEnd(Unit* target, EatDrinkSession const& session, uint32 elapsedMs);

    private:
        void HandleToggle(Aura* aura, bool apply, EatDrinkKind kind,
                           std::unordered_map<ObjectGuid, EatDrinkSession>& sessions);

        static EatDrinkSource DetermineSource(uint32 itemEntry);
        static uint32 GetCastItemEntry(Aura* aura, Unit* target);
        static char const* KindToString(EatDrinkKind kind);
        static char const* SourceToString(EatDrinkSource source);

        std::unordered_map<ObjectGuid, EatDrinkSession> m_foodSessions;
        std::unordered_map<ObjectGuid, EatDrinkSession> m_drinkSessions;

        std::unordered_map<ObjectGuid, uint32> m_pendingItems;
};

#define sEatDrinkTracker EatDrinkTracker::Instance()

#endif // MANGOS_EAT_DRINK_TRACKER_H
