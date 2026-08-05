// EatDrinkTracker.cpp

#include "EatDrinkTracker.h"

#include "Entities/Unit.h"
#include "Entities/Player.h"
#include "Entities/Item.h"
#include "Globals/ObjectMgr.h"          // sObjectMgr, GetItemPrototype
#include "Spells/SpellAuras.h"          // Aura, SpellAuraHolder
#include "Log/Log.h"

#include "ClassicPlusSystems/Vitality/VitalityMgr.h"

EatDrinkTracker& EatDrinkTracker::Instance()
{
    static EatDrinkTracker instance;
    return instance;
}

void EatDrinkTracker::OnFoodAuraToggled(Aura* aura, bool apply)
{
    HandleToggle(aura, apply, EatDrinkKind::Food, m_foodSessions);
}

void EatDrinkTracker::OnDrinkAuraToggled(Aura* aura, bool apply)
{
    HandleToggle(aura, apply, EatDrinkKind::Drink, m_drinkSessions);
}

void EatDrinkTracker::HandleToggle(Aura* aura, bool apply, EatDrinkKind kind,
                                    std::unordered_map<ObjectGuid, EatDrinkSession>& sessions)
{
    if (!aura)
        return;

    Unit* target = aura->GetTarget();
    if (!target)
        return;

    ObjectGuid const guid = target->GetObjectGuid();

    if (apply)
    {
        // Guard against being called twice for the same logical session
        // (e.g. a refresh that removes+reapplies internally). If you find
        // that happens in your branch, this simply overwrites with a new
        // start rather than double-counting.
        EatDrinkSession session;
        session.kind = kind;
        session.spellId = aura->GetId();
        session.startTime = time(nullptr);

        if (SpellAuraHolder* holder = aura->GetHolder())
            session.maxDurationMs = static_cast<uint32>(holder->GetAuraMaxDuration());

        //session.itemEntry = GetCastItemEntry(aura, target);
        session.itemEntry = ConsumePendingItem(target->GetObjectGuid());
        session.source = DetermineSource(session.itemEntry);

        sessions[guid] = session;

        OnStart(target, session);
    }
    else
    {
        auto it = sessions.find(guid);
        if (it == sessions.end())
            return; // no matching start on record; ignore stray remove

        uint32 const elapsedMs = static_cast<uint32>(difftime(time(nullptr), it->second.startTime) * 1000);

        OnEnd(target, it->second, elapsedMs);

        sessions.erase(it);
    }

    // -------------------

    //char debug[64];

    //snprintf(debug, sizeof(debug), "Toggle: %s", KindToString(kind));

    //static_cast<Player*>(target)->GetSession()->SendNotification(debug);

    //--------------------

}

void EatDrinkTracker::RegisterPendingItem(ObjectGuid guid, uint32 itemEntry)
{
    m_pendingItems[guid] = itemEntry;
}

uint32 EatDrinkTracker::ConsumePendingItem(ObjectGuid guid)
{
    auto itr = m_pendingItems.find(guid);

    if (itr == m_pendingItems.end())
        return 0;

    uint32 item = itr->second;

    m_pendingItems.erase(itr);

    return item;
}

uint32 EatDrinkTracker::GetCastItemEntry(Aura* aura, Unit* target)
{
    /*
    if (castItemGuid.IsEmpty())
    {
        sLog.outString("EatDrink: CastItemGuid EMPTY");
        return 0;
    }

    Item* item = static_cast<Player*>(target)->GetItemByGuid(castItemGuid);

    if (!item)
    {
        sLog.outString("EatDrink: Item not found from GUID");
        return 0;
    }

    sLog.outString("EatDrink: Item entry %u", item->GetEntry());

    return item->GetEntry();
    */


    SpellAuraHolder* holder = aura->GetHolder();
    if (!holder)
        return 0;

    ObjectGuid const castItemGuid = holder->GetCastItemGuid();
    if (castItemGuid.IsEmpty())
        return 0;

    if (target->GetTypeId() != TYPEID_PLAYER)
        return 0;

    // NOTE: this only works reliably if called at aura-apply time, i.e.
    // before Spell::TakeCastItem() consumes the item's last charge.
    Item* item = static_cast<Player*>(target)->GetItemByGuid(castItemGuid);
    
    return item ? item->GetEntry() : 0;
  
}

EatDrinkSource EatDrinkTracker::DetermineSource(uint32 itemEntry)
{
    
    if (itemEntry == 0)
        return EatDrinkSource::Other;

    ItemPrototype const* proto = sObjectMgr.GetItemPrototype(itemEntry);
    if (!proto)
        return EatDrinkSource::Other;

    // Adjust the flag name below if your tree spells it differently
    // (ITEM_FLAG_CONJURED / ITEM_FLAGS_CONJURED / ITEM_PROTO_FLAG_CONJURED).
    if (proto->Flags & ITEM_FLAG_CONJURED)
        return EatDrinkSource::Conjured;

    return EatDrinkSource::Regular;
    
}

char const* EatDrinkTracker::KindToString(EatDrinkKind kind)
{
    return kind == EatDrinkKind::Food ? "food" : "drink";
}

char const* EatDrinkTracker::SourceToString(EatDrinkSource source)
{
    switch (source)
    {
        case EatDrinkSource::Conjured: return "conjured";
        case EatDrinkSource::Regular:  return "regular";
        default:                       return "other";
    }
}

void EatDrinkTracker::OnStart(Unit* target, EatDrinkSession const& session)
{
    sLog.outString("[EatDrink] START guid=%u spell=%u kind=%s source=%s item=%u maxDuration=%ums",
                    target->GetGUIDLow(), session.spellId, KindToString(session.kind),
                    SourceToString(session.source), session.itemEntry, session.maxDurationMs);

    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = static_cast<Player*>(target);

    // -----------------------

    char buffer[128];

    snprintf(buffer,
        sizeof(buffer),
        "Item=%u Source=%u",
        session.itemEntry,
        static_cast<uint32>(session.source));

    player->GetSession()->SendNotification(buffer);

    // -----------------------

    switch (session.kind)
    {
        case EatDrinkKind::Food:
            sVitalityMgr.BeginEat(player, session);
            break;

        case EatDrinkKind::Drink:
            sVitalityMgr.BeginDrink(player, session);
            break;
    }

}

void EatDrinkTracker::OnEnd(Unit* target, EatDrinkSession const& session, uint32 elapsedMs)
{
    sLog.outString("[EatDrink] END guid=%u spell=%u kind=%s source=%s item=%u elapsed=%ums (maxDuration=%ums)",
                    target->GetGUIDLow(), session.spellId, KindToString(session.kind),
                    SourceToString(session.source), session.itemEntry, elapsedMs, session.maxDurationMs);

    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = static_cast<Player*>(target);

    //player->GetSession()->SendNotification("EatDrinkTracker::OnEnd()"); // used for test/debug

    switch (session.kind)
    {
        case EatDrinkKind::Food:
            sVitalityMgr.EndEat(player, session, elapsedMs);
            break;

        case EatDrinkKind::Drink:
            sVitalityMgr.EndDrink(player, session, elapsedMs);
            break;
    }

    char debug[128];

    snprintf(debug,
        sizeof(debug),
        "Food source = %u",
        static_cast<uint32>(session.source));

    player->GetSession()->SendNotification(debug);

}
