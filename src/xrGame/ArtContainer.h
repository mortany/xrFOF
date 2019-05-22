////////////////////////////////////////////////////////////////////////////
//	Desc        : Main class for artefact's containers
//	Author		: Mortan
////////////////////////////////////////////////////////////////////////////
//  Forward Frontier Team 2016-2019
////////////////////////////////////////////////////////////////////////////

#pragma once
#include "inventory_item_object.h"
#include "inventory_space.h"

class CArtefactContainer : public CInventoryItemObject
{
protected:
    typedef CInventoryItemObject inherited;
public:
    xr_vector<u16> m_items;
    xr_vector<shared_str> m_items_name;

protected:
    u8 avaible_slots;
    u8 protect_lvl;
public:
    CArtefactContainer();
    virtual ~CArtefactContainer();
    virtual void OnEvent(NET_Packet& P, u16 type);
    virtual BOOL net_Spawn(CSE_Abstract* DC);
    virtual void net_Destroy();
    virtual void Load(LPCSTR section);
    bool IsHasFreeSlot() { return avaible_slots > 0; }
    u8 GetFreeSlots() { return avaible_slots; }
    virtual float Weight() const;
    void AddAvailableItems(TIItemContainer& items_container) const;
    IC bool IsEmpty() const { return m_items.empty(); }
};
