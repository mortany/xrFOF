#include "StdAfx.h"
#include "UIBoosterInfo.h"
#include "UIContainerInfo.h"
#include "UIHelper.h"

CUI_FFT_ItemsInfo::CUI_FFT_ItemsInfo() 
{ 
    for (u32 i = 0; i < 3; ++i)
    {
        m_avaible_slots[i] = NULL;
    }
}

CUI_FFT_ItemsInfo::~CUI_FFT_ItemsInfo() 
{ 
    delete_data(m_avaible_slots);
    xr_delete(m_Prop_line);
}

void CUI_FFT_ItemsInfo::InitFromXml(CUIXml& xml)
{ 
    LPCSTR base = "fft_items_params";
    XML_NODE stored_root = xml.GetLocalRoot();
    XML_NODE base_node = xml.NavigateToNode(base, 0);
    if (!base_node)
        return;

    CUIXmlInit::InitWindow(xml, base, 0, this);
    xml.SetLocalRoot(base_node);

    m_Prop_line = new CUIStatic();
    AttachChild(m_Prop_line);
    m_Prop_line->SetAutoDelete(false);
    CUIXmlInit::InitStatic(xml, "prop_line", 0, m_Prop_line);

    for (u32 i = 0; i < 3; ++i)
    {
        m_avaible_slots[i] = new CUI_FFT_Item();
        m_avaible_slots[i]->SetAutoDelete(false);
        m_avaible_slots[i]->Init(xml, "avaible_slots");
        shared_str name = StringTable().translate("ui_container_slot");
        m_avaible_slots[i]->SetCaption(name.c_str());
        xml.SetLocalRoot(base_node);
    }

    xml.SetLocalRoot(stored_root);
}

void CUI_FFT_ItemsInfo::SetInfo(shared_str const& section, xr_vector<shared_str> &arts_item)
{
    DetachAll();
    AttachChild(m_Prop_line);

    Fvector2 pos;
    float h = m_Prop_line->GetWndPos().y + m_Prop_line->GetWndSize().y;

    if (arts_item.size() != 0)
    {
        auto it = arts_item.begin();
        u8 size = 0;

        for (; it != arts_item.end(); it++)
        {
            size = it - arts_item.begin();
            shared_str art_name = StringTable().translate(pSettings->r_string((*it).c_str(), "inv_name"));
            u8 lvl = READ_IF_EXISTS(pSettings, r_u8, (*it).c_str(), "level_item", 0);
            u32 color = color_rgba(170, 170, 170, 255);
            switch (lvl)
            {
                case 1: color = color_rgba(187, 255, 255, 255); break;
                
                case 2: color = color_rgba(118, 238, 198, 255); break;
                
                case 3: color = color_rgba(0, 205, 102, 255); break;
                
                case 4: color = color_rgba(145, 44, 238, 255); break;
                
                case 5: color = color_rgba(180, 40, 35, 255); break;
                
                case 0: break;

                default: break;
            }
            m_avaible_slots[size]->get_caption()->SetTextColor(color);
            m_avaible_slots[size]->SetValue(art_name.c_str());
            pos.set(m_avaible_slots[size]->GetWndPos());
            pos.y = h;
            m_avaible_slots[size]->SetWndPos(pos);
            h += m_avaible_slots[size]->GetWndSize().y;
            AttachChild(m_avaible_slots[size]);
        }
    }

    SetHeight(h);
}

CUI_FFT_Item::CUI_FFT_Item()
{
    m_caption = NULL;
    m_value = NULL;
    m_texture._set("");
}

CUI_FFT_Item::~CUI_FFT_Item() {}

void CUI_FFT_Item::Init(CUIXml& xml, LPCSTR section)
{
    CUIXmlInit::InitWindow(xml, section, 0, this);
    xml.SetLocalRoot(xml.NavigateToNode(section));

    m_caption = UIHelper::CreateStatic(xml, "caption", this);
    m_value = UIHelper::CreateTextWnd(xml, "value", this);    

    LPCSTR texture_plus = xml.Read("caption:texture", 0, "");
    m_texture._set(texture_plus);

}

void CUI_FFT_Item::SetValue(LPCSTR value)
{
    m_value->SetText(value);
    m_caption->InitTexture(m_texture.c_str());
}

void CUI_FFT_Item::SetCaption(LPCSTR name) { m_caption->TextItemControl()->SetText(name); }
