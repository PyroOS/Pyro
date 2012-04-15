#ifndef VIEW_H
#define VIEW_H

#include <gui/view.h>

using namespace os;


class RandView : public View
{
public:
	RandView();
	
	void Paint(const os::Rect&);
	virtual os::Point GetPreferredSize(bool) const;
	void Refresh();
	
	void SetRandomBackColor(bool color);
	void SetRandomFont(bool bFont);
	void SetRandomFontColor(bool bFntColor);
	void SetRandomTextType(uint nType);
	bool GetRandBackColor();
	bool GetRandomFont();
	bool GetRandomFontColor();
	uint GetStringType();
private:
	os::Font* m_pcFont;
	Color32_s m_sFontColor;
	
	bool m_bBackColor;
	bool m_bFont;
	bool m_bFontColor;
	uint m_nStringType;
	Color32_s m_sBackColor;
};

#endif
