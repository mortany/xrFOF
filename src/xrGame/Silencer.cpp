///////////////////////////////////////////////////////////////
// Silencer.cpp
// Silencer - апгрейд оружия глушитель
///////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "Silencer.h"

CSilencer::CSilencer() 
{
    iMaxShots = 0;
    iCurrentShots = 0;
    bCanBeBroken = false;
    bShotsIsLoaded = false;
}

CSilencer::~CSilencer() {}

BOOL CSilencer::net_Spawn(CSE_Abstract* DC) 
{ 
    BOOL bResult = inherited::net_Spawn(DC);

    if(!bShotsIsLoaded && bCanBeBroken)
        iCurrentShots = iMaxShots;

    return bResult;
}

void CSilencer::Load(LPCSTR section) 
{ 
    inherited::Load(section);
    iMaxShots = READ_IF_EXISTS(pSettings, r_u32, section, "max_shots", 0);
    bCanBeBroken = iMaxShots > 0;
}

void CSilencer::OnEvent(NET_Packet& P, u16 type)
{
    switch (type)
    {
    case GE_ADDON_STATES_UPDATE:
    {
        P.r_u32(iCurrentShots);
        clamp(iCurrentShots, (u32)0, iMaxShots);
    }
    default:
    {
        inherited::OnEvent(P, type);
    }break;
    }
}

void CSilencer::save(NET_Packet& output_packet)
{
    inherited::save(output_packet);
    save_data(iCurrentShots, output_packet);
}

void CSilencer::load(IReader& input_packet)
{
    inherited::load(input_packet);
    load_data(iCurrentShots, input_packet);
    bShotsIsLoaded = true;
}

void CSilencer::net_Destroy() { inherited::net_Destroy(); }
void CSilencer::UpdateCL() { inherited::UpdateCL(); }
void CSilencer::OnH_A_Chield() { inherited::OnH_A_Chield(); }
void CSilencer::OnH_B_Independent(bool just_before_destroy) { inherited::OnH_B_Independent(just_before_destroy); }
void CSilencer::renderable_Render() { inherited::renderable_Render(); }
