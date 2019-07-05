#include "pch_script.h"
#include "Scope.h"
#include "Silencer.h"
#include "GrenadeLauncher.h"
#include "xrScriptEngine/ScriptExporter.hpp"

CScope::CScope() 
{
    current_mark = 0;
}
CScope::~CScope() {}
using namespace luabind;

void CScope::Load(LPCSTR section)
{
    inherited::Load(section);
}

void CScope::OnEvent(NET_Packet& P, u16 type)
{
    switch (type)
    {
        case GE_ADDON_STATES_UPDATE:
        {
            u8 p;
            P.r_u8(current_mark);
        }
        default: 
        { 
            inherited::OnEvent(P, type);
        }break;  
    }
}

void CScope::save(NET_Packet& output_packet)
{
    inherited::save(output_packet);
    save_data(current_mark, output_packet);
}

void CScope::load(IReader& input_packet)
{
    inherited::load(input_packet);
    load_data(current_mark, input_packet);
}

SCRIPT_EXPORT(CScope, (CGameObject), {
    module(luaState)[class_<CScope, CGameObject>("CScope").def(constructor<>()),

        class_<CSilencer, CGameObject>("CSilencer").def(constructor<>()),

        class_<CGrenadeLauncher, CGameObject>("CGrenadeLauncher").def(constructor<>())];
});
