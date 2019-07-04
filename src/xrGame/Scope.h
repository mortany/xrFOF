///////////////////////////////////////////////////////////////
// Scope.h
// Scope - апгрейд оружия снайперский прицел
///////////////////////////////////////////////////////////////

#pragma once

#include "inventory_item_object.h"

class CScope : public CInventoryItemObject
{
private:
    typedef CInventoryItemObject inherited;

public:
    CScope();
    virtual ~CScope();
    virtual void Load(LPCSTR section);
    virtual void OnEvent(NET_Packet& P, u16 type);
    virtual void save(NET_Packet& output_packet);
    virtual void load(IReader& input_packet);
    u8 current_mark;
};
