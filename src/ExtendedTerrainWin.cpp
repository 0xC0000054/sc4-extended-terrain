////////////////////////////////////////////////////////////////////////////
//
// This file is part of sc4-extended-terrain, a DLL Plugin for SimCity 4
// that adds support for more terrain types.
//
// Copyright (c) 2024, 2025 Nicholas Hayes
//
// This file is licensed under terms of the MIT License.
// See LICENSE.txt for more information.
//
////////////////////////////////////////////////////////////////////////////

#include "ExtendedTerrainWin.h"
#include "DebugUtil.h"
#include "Logger.h"
#include "cGZMessage.h"
#include "cIGZLanguageManager.h"
#include "cIGZLanguageUtility.h"
#include "cIGZWin.h"
#include "cIGZWinCombo.h"
#include "cIGZWinGen.h"
#include "cIGZWinText.h"
#include "cISC4App.h"
#include "cISC4Region.h"
#include "cISC4RegionalCity.h"
#include "cGZPersistResourceKey.h"
#include "cRZAutoRefCount.h"
#include "SC4UI.h"
#include "StringResourceKey.h"
#include "StringResourceManager.h"
#include "GZServPtrs.h"

namespace
{
	void SetWinProc(cIGZWin* pWin, cIGZWinProc* pWinProc)
	{
		cRZAutoRefCount<cIGZWinGen> winGen;

		if (pWin->QueryInterface(GZIID_cIGZWinGen, winGen.AsPPVoid()))
		{
			winGen->SetWinProc(pWinProc);
		}
	}

	constexpr uint32_t kCloseDialogButtonID = 0x52DE7AB7;
	constexpr uint32_t kTerrainComboBoxID = 0x52DE7ABA;

	void DebugLogWinProcMessage(const cGZMessage& msg)
	{
#ifdef _DEBUG
		const char* description = nullptr;
		bool log = true;

		switch (msg.dwMessageType)
		{
		case 3:
			description = "on_command";
			break;
		case 0xA2BF8AD5:
			description = "on_init";
			break;
		case 0xa2bf8ad6:
			description = "on_shutdown";
			break;
		case 0xA2BF8AD8:
			description = "on_layout";
			break;
		case 0xa2bf8acd:
			description = "begin_paint";
			log = false;
			break;
		case 0xa2bf8ace:
			description = "on_paint";
			log = false;
			break;
		case 0xa2bf8acf:
			description = "end_paint";
			log = false;
			break;
		case 0xa2bf8ad4:
			description = "GZWinGen_point_in_Window_coordinates";
			log = false;
			break;
		case 0x4348B111:
			description = "app_gain_loose_focus";
			break;
		}

		if (log)
		{
			if (description)
			{
				DebugUtil::PrintLineToDebugOutputFormatted(
					"%s (0x%08X), data1=0x%08X, data2=0x%08X, data3=0x%08X",
					description,
					msg.dwMessageType,
					msg.dwData1,
					msg.dwData2,
					msg.dwData3);
			}
			else
			{
				DebugUtil::PrintLineToDebugOutputFormatted(
					"type=0x%08X, data1=0x%08X, data2=0x%08X, data3=0x%08X",
					msg.dwMessageType,
					msg.dwData1,
					msg.dwData2,
					msg.dwData3);
			}
		}
#endif // _DEBUG
	}
}

ExtendedTerrainWin::ExtendedTerrainWin(IExtendedTerrainWinManager& pWinMgr)
	: pExtendedTerrainWin(nullptr),
	  winMgr(pWinMgr)
{
}

bool ExtendedTerrainWin::Create(
	cIGZWin* const pParentWin,
	const std::vector<TerrainEntry>& terrainNames,
	const cRZBaseString& currentTerrainName)
{
	if (!pExtendedTerrainWin)
	{
		// This is the TGI of our Extended Terrain window.
		cGZPersistResourceKey key(0x00000000, 0xb8588da4, 0x2be83d33);
		cRZAutoRefCount<cIGZWin> scriptWin;

		if (!SC4UI::CreateWindowFromScript(
			key,
			pParentWin,
			0x52DE7AB6,
			GZIID_cIGZWin,
			reinterpret_cast<void**>(&pExtendedTerrainWin)))
		{
			if (pExtendedTerrainWin)
			{
				pExtendedTerrainWin->Release();
				pExtendedTerrainWin = nullptr;
			}

			Logger::GetInstance().WriteLine(
				LogLevel::Error,
				"Failed to create the extended terrain window, is ExtendedTerrainUI.dat in the Plugins folder?");

			return false;
		}

		pParentWin->ChildAdd(pExtendedTerrainWin);

		// The WinProc is set before the combo box because the combo box
		// may send notifications during its initialization.
		SetWinProc(pExtendedTerrainWin, this);

		cRZAutoRefCount<cIGZWinCombo> pCombo;

		if (pExtendedTerrainWin->GetChildAsRecursive(kTerrainComboBoxID, GZIID_cIGZWinCombo, pCombo.AsPPVoid()))
		{
			pCombo->RemoveAllStrings();

			for (const auto& item : terrainNames)
			{
				pCombo->InsertString(item.GetSectionPrefix(), -2);
			}

			if (terrainNames.size() > 1)
			{
				int32_t index = 0;

				if (pCombo->GetIndexFromString(index, currentTerrainName))
				{
					pCombo->SetSelection(index, false);
				}
				else
				{
					pCombo->SetSelection(0, true);
				}
			}
			else
			{
				pCombo->SetSelection(0, false);
			}
		}

		const int32_t parentWindowWidth = pParentWin->GetW();
		const int32_t parentWindowHeight = pParentWin->GetH();
		const int32_t extendedTerrainWindowWidth = pExtendedTerrainWin->GetW();
		const int32_t extendedTerrainWindowHeight = pExtendedTerrainWin->GetH();

		// This places the dialog in the center of the region view screen.

		const int32_t x = (parentWindowWidth / 2) - (extendedTerrainWindowWidth / 2);
		const int32_t y = (parentWindowHeight / 2) - (extendedTerrainWindowHeight / 2);

		pExtendedTerrainWin->GZWinMoveTo(x, y);

		pExtendedTerrainWin->PullToFront();
		pExtendedTerrainWin->ShowWindow();
	}

	return true;
}

bool ExtendedTerrainWin::Destroy(cIGZWin& parentWin)
{
	if (pExtendedTerrainWin)
	{
		parentWin.ChildDelete(pExtendedTerrainWin);

		SetWinProc(pExtendedTerrainWin, nullptr);

		pExtendedTerrainWin->Release();
		pExtendedTerrainWin = nullptr;
	}

	return true;
}

bool ExtendedTerrainWin::WindowCreated() const
{
	return pExtendedTerrainWin != nullptr;
}

void ExtendedTerrainWin::ToggleWindowVisibility()
{
	if (pExtendedTerrainWin)
	{
		if (pExtendedTerrainWin->IsVisible())
		{
			pExtendedTerrainWin->HideWindow();
		}
		else
		{
			pExtendedTerrainWin->ShowWindow();
		}
	}
}

bool ExtendedTerrainWin::DoWinProcMessage(cIGZWin* pWin, cGZMessage& msg)
{
	constexpr uint32_t kGZWin_MessageType_Command = 3;

	constexpr uint32_t kGZWin_Command_ButtonClicked = 0x287259F6;
	constexpr uint32_t kGZWin_Command_ComboSelectionChanged = 0xE8711255;

	bool result = false;

	if (msg.dwMessageType == kGZWin_MessageType_Command)
	{
		const uint32_t commandID = msg.dwData1;
		const uint32_t controlID = msg.dwData2;

		if (commandID == kGZWin_Command_ButtonClicked)
		{
			if (controlID == kCloseDialogButtonID)
			{
				pExtendedTerrainWin->HideWindow();
				winMgr.WindowClosed();
				result = true;
			}
		}
		else if (commandID == kGZWin_Command_ComboSelectionChanged)
		{
			if (controlID == kTerrainComboBoxID)
			{
				cRZAutoRefCount<cIGZWinCombo> pCombo;

				if (pExtendedTerrainWin->GetChildAsRecursive(kTerrainComboBoxID, GZIID_cIGZWinCombo, pCombo.AsPPVoid()))
				{
					winMgr.SetCurrentTerrainName(*pCombo);
				}
			}
		}
	}


	return result;
}
