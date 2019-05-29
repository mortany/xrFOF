#pragma once
#include "xrUICore/Windows/UIWindow.h"
#include "xrServerEntities/alife_space.h"

class CUIXml;
class CUIStatic;
class CUITextWnd;
class UIArtefactParamItem;

class CUIArtefactParams : public CUIWindow
{
public:
    CUIArtefactParams();
    virtual ~CUIArtefactParams();
    void InitFromXml(CUIXml& xml);
    bool Check(const shared_str& af_section);
    void SetInfo(const shared_str& af_section,float charge = 0.0f, bool can_charge = false);

protected:
    UIArtefactParamItem* m_immunity_item[ALife::infl_max_count];
    UIArtefactParamItem* m_restore_item[ALife::eRestoreTypeMax];
    UIArtefactParamItem* m_additional_weight;
    UIArtefactParamItem* m_charge_value;
    Fvector2 pos_prop;

    CUIStatic* m_Prop_line;

}; // class CUIArtefactParams

// -----------------------------------

class UIArtefactParamItem : public CUIWindow
{
public:
    UIArtefactParamItem();
    virtual ~UIArtefactParamItem();

    void Init(CUIXml& xml, LPCSTR section);
    void SetCaption(LPCSTR name);
    void SetValue(float value, bool need_sign = true);

//private:
    CUIStatic* m_caption;
    CUITextWnd* m_value;
    float m_magnitude;
    bool m_sign_inverse;
    shared_str m_unit_str;
    shared_str m_texture_minus;
    shared_str m_texture_plus;

}; // class UIArtefactParamItem
