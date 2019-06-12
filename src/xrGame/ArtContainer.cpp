////////////////////////////////////////////////////////////////////////////
//	Desc        : Main class for artefact's containers
//	Author		: Mortan
////////////////////////////////////////////////////////////////////////////
//  Forward Frontier Team 2016-2019
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "StdAfx.h"
#include "ArtContainer.h"

CArtefactContainer::CArtefactContainer()
{
    avaible_slots = 0;
    protect_lvl = 0;
}

CArtefactContainer::~CArtefactContainer() {}

void CArtefactContainer::AddAvailableItems(TIItemContainer& items_container) const
{
    for (const u16 &it : m_items)
    {
        PIItem itm = smart_cast<PIItem>(Level().Objects.net_Find(it));
        VERIFY(itm);
        items_container.push_back(itm);
    }
}

void CArtefactContainer::net_Destroy() { inherited::net_Destroy(); }

float CArtefactContainer::Weight() const 
{
    float weight = CInventoryItemObject::Weight();

    for (const u16 &it : m_items)
    {
        PIItem itm = smart_cast<PIItem>(Level().Objects.net_Find(it));
        VERIFY(itm);
        weight += itm->Weight();
    }

    return weight;
}

void CArtefactContainer::Load(LPCSTR section)
{
    inherited::Load(section);
    avaible_slots = READ_IF_EXISTS(pSettings, r_u8, section, "avaible_slots", 1);
    clamp(avaible_slots, (u8)1, avaible_slots);
}

BOOL CArtefactContainer::net_Spawn(CSE_Abstract* DC)
{
    BOOL bResult = inherited::net_Spawn(DC);
    return bResult;
}

void CArtefactContainer::OnEvent(NET_Packet& P, u16 type)
{
    inherited::OnEvent(P, type);

    switch (type)
    {
    case GE_TRADE_BUY:
    case GE_OWNERSHIP_TAKE:
    {
        u16 id;
        P.r_u16(id);
        IGameObject* itm = Level().Objects.net_Find(id);
        VERIFY(itm);
        m_items.push_back(id);
        m_items_name.push_back(itm->cNameSect().c_str());
        itm->H_SetParent(this);
        itm->setVisible(FALSE);
        itm->setEnabled(FALSE);
        CInventoryItem* pIItem = smart_cast<CInventoryItem*>(itm);
        VERIFY(pIItem);
        --avaible_slots;
    }
    break;
    case GE_TRADE_SELL:
    case GE_OWNERSHIP_REJECT:
    {
        u16 id;
        P.r_u16(id);
        IGameObject* itm = Level().Objects.net_Find(id);
        VERIFY(itm);
        xr_vector<u16>::iterator it;
        it = std::find(m_items.begin(), m_items.end(), id);
        VERIFY(it != m_items.end());
        m_items.erase(it);
        xr_vector<shared_str>::iterator it_;
        it_ = std::find(m_items_name.begin(), m_items_name.end(), itm->cNameSect().c_str());
        m_items_name.erase(it_);
        bool just_before_destroy = !P.r_eof() && P.r_u8();
        bool dont_create_shell = (type == GE_TRADE_SELL) || just_before_destroy;

        itm->H_SetParent(NULL, dont_create_shell);
        ++avaible_slots;
    }
    break;
    };
}
