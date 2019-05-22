#pragma once
#include "xrUICore/Windows/UIWindow.h"

class CUIXml;
class CUIStatic;
class CUITextWnd;
class CUI_FFT_Item;

class CUI_FFT_ItemsInfo : public CUIWindow
{
public:
    CUI_FFT_ItemsInfo();
    virtual ~CUI_FFT_ItemsInfo();
    void InitFromXml(CUIXml& xml);
    void SetInfo(const shared_str& section,xr_vector<shared_str>& arts_item);

protected:
    CUI_FFT_Item* m_avaible_slots[3];
    CUIStatic* m_Prop_line;

}; // class CUI_FFT_ItemsInfo

class CUI_FFT_Item : public CUIWindow
{
public:
    CUI_FFT_Item();
    virtual ~CUI_FFT_Item();

    void Init(CUIXml& xml, LPCSTR section);
    void SetCaption(LPCSTR name);
    void SetValue(LPCSTR value);
    CUITextWnd* get_caption() { return m_value; }

private:
    CUIStatic* m_caption;
    CUITextWnd* m_value;
    shared_str m_texture;

}; // class UIBoosterInfoItem
