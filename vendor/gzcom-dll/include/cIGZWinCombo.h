#pragma once
#include "cIGZUnknown.h"

class cIGZBuffer;
class cIGZFont;
class cIGZString;
class cIGZWin;
class cIGZWinTextEdit;
class cRZRect;

static const uint32_t GZIID_cIGZWinCombo = 0x412ce496;

class cIGZWinCombo : public cIGZUnknown
{
public:
	enum class FontColorIndex : int32_t
	{
		Normal = 0,
		Disabled = 1,
		Highlighted = 2
	};

	virtual cIGZWin* AsIGZWin() = 0;

	virtual cIGZWinTextEdit* GetLineInput() = 0;
	virtual cIGZWinTextEdit* GetTextEdit() = 0;

	virtual void SetFlag(int32_t) = 0;
	virtual void SetFlags(int32_t) = 0;
	virtual void ClearFlag(int32_t) = 0;
	virtual bool IsFlagSet(int32_t) = 0;
	virtual int32_t GetFlags() = 0;

	virtual void SetFont(cIGZFont*) = 0;
	virtual bool SetFontColor(FontColorIndex, uint32_t) = 0;
	virtual uint32_t GetFontColor(FontColorIndex) const = 0;
	virtual bool GetFontColor(FontColorIndex, uint32_t&) const = 0;
	virtual void ClearFontColor(FontColorIndex) = 0;

	virtual bool SetHighlightColor(uint32_t) = 0;
	virtual bool GetHighlightColor(uint32_t&) const = 0;
	virtual bool SetHighlightFillAlpha(float) = 0;
	virtual bool GetHighlightFillAlpha(float&) const = 0;

	virtual bool GetEditable() const = 0;
	virtual bool SetEditable(bool) = 0;

	virtual bool SetDropDownOutlineColor(uint32_t) = 0;
	virtual bool GetDropDownOutlineColor(uint32_t&) const = 0;

	virtual bool RemoveString(cIGZString const&) = 0;
	virtual bool RemoveString(int32_t) = 0;
	virtual void RemoveAllStrings() = 0;
	virtual bool InsertString(cIGZString const&, int32_t) = 0;

	virtual bool SetSelection(int32_t index, bool sendChangedNotification) = 0;
	virtual int32_t GetSelection() const = 0;
	virtual bool GetSelection(int32_t&, cIGZString&) const = 0;

	virtual bool GetStringFromIndex(int32_t, cIGZString&) const = 0;
	virtual bool GetIndexFromString(int32_t&, cIGZString const&) const = 0;

	virtual bool SetComboDownArrow(cIGZBuffer*, cRZRect const*) = 0;
	virtual bool GetComboDownArrow(cIGZBuffer*&, cRZRect&) const = 0;
	virtual bool SetComboDownColor(uint32_t) = 0;
	virtual bool GetComboDownColor(uint32_t&) const = 0;

	virtual bool SetBtnGutter(int8_t) = 0;
	virtual bool GetBtnGutter(int8_t&) const = 0;
	virtual bool SetTextGutters(int8_t, int8_t) = 0;
	virtual bool GetTextGutters(int8_t&, int8_t&) const = 0;

	virtual bool GetMinMaxInfo(cRZRect&) = 0;
};