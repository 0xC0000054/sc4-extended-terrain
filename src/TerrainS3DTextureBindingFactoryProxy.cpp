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

#include "TerrainS3DTextureBindingFactoryProxy.h"
#include "cGZPersistResourceKey.h"
#include "cIGZCOM.h"
#include "cIGZFrameWork.h"
#include "cRZCOMDllDirector.h"
#include "cS3DTextureBinding.h"
#include "GZServPtrs.h"
#include "Logger.h"


TerrainS3DTextureBindingFactoryProxy::TerrainS3DTextureBindingFactoryProxy(
	const ISettings& settings,
	const ITerrainTextureRedirectManager& textureRedirectManager)
	: cRZBaseSystemService(GZSRVID_TerrainS3DTextureBindingFactoryProxy, 0x130ee8),
	  pFactory(nullptr),
	  settings(settings),
	  textureRedirectManager(textureRedirectManager)
{
}

bool TerrainS3DTextureBindingFactoryProxy::IsInitialized() const
{
	return pFactory != nullptr;
}

bool TerrainS3DTextureBindingFactoryProxy::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_cIS3DTextureBindingFactory)
	{
		*ppvObj = static_cast<cIS3DTextureBindingFactory*>(this);
		AddRef();

		return true;
	}
	else if (riid == GZIID_ITerrainS3DTextureBindingFactoryProxy)
	{
		*ppvObj = static_cast<ITerrainS3DTextureBindingFactoryProxy*>(this);
		AddRef();

		return true;
	}

	return cRZBaseSystemService::QueryInterface(riid, ppvObj);
}

uint32_t TerrainS3DTextureBindingFactoryProxy::AddRef()
{
	return cRZBaseSystemService::AddRef();
}

uint32_t TerrainS3DTextureBindingFactoryProxy::Release()
{
	return cRZBaseSystemService::Release();
}

bool TerrainS3DTextureBindingFactoryProxy::Init()
{
	bool result = false;

	cIS3DTextureBindingFactoryPtr textureBinding;
	pFactory = textureBinding;

	if (pFactory)
	{
		textureBinding->AddRef();
		result = true;
	}

	return result;
}

bool TerrainS3DTextureBindingFactoryProxy::Shutdown()
{
	cIS3DTextureBindingFactory* localFactory = pFactory;

	if (localFactory)
	{
		localFactory->Release();
		pFactory = nullptr;
	}

	return true;
}

bool TerrainS3DTextureBindingFactoryProxy::SetGDriver(cIGZGDriver* pDriver)
{
	return pFactory->SetGDriver(pDriver);
}

bool TerrainS3DTextureBindingFactoryProxy::GetBinding(cGZPersistResourceKey const& originalResKey, cS3DTextureBinding** ppTextureBinding)
{
	const cGZPersistResourceKey key(
		originalResKey.type,
		originalResKey.group,
		textureRedirectManager.GetRedirectedTextureInstanceID(originalResKey.instance));

	if (settings.LogLoadedTextureIDs())
	{
		Logger::GetInstance().WriteLineFormatted(
			LogLevel::Info,
			"Requested texture 0x%08X,0x%08X,0x%08X",
			key.type,
			key.group,
			key.instance);
	}

	bool result = pFactory->GetBinding(key, ppTextureBinding);

	if (!result && settings.LogTextureLoadErrors())
	{
		Logger::GetInstance().WriteLineFormatted(
			LogLevel::Info,
			"Failed to load texture 0x%08X,0x%08X,0x%08X",
			key.type,
			key.group,
			key.instance);
	}

	return result;
}

bool TerrainS3DTextureBindingFactoryProxy::GetBinding(uint32_t iid, cS3DTextureBinding** ppTextureBinding)
{
	return pFactory->GetBinding(iid, ppTextureBinding);
}

bool TerrainS3DTextureBindingFactoryProxy::GetBinding(cS3DTextureBufferInfo* pInfo, cS3DTextureBinding** ppTextureBinding)
{
	return pFactory->GetBinding(pInfo, ppTextureBinding);
}

void TerrainS3DTextureBindingFactoryProxy::SetBindingImageData(cS3DTextureBinding* pBinding, cS3DTextureBufferInfo* pInfo)
{
	pFactory->SetBindingImageData(pBinding, pInfo);
}

void TerrainS3DTextureBindingFactoryProxy::GetBindingInfo(cS3DTextureBinding* pBinding, cS3DTextureBindingInfo* pInfo)
{
	pFactory->GetBindingInfo(pBinding, pInfo);
}

void TerrainS3DTextureBindingFactoryProxy::SetLodOffset(uint32_t offset)
{
	pFactory->SetLodOffset(offset);
}

void TerrainS3DTextureBindingFactoryProxy::SetInternalTextureBitDepth(uint32_t bitDepth)
{
	pFactory->SetInternalTextureBitDepth(bitDepth);
}

uint32_t TerrainS3DTextureBindingFactoryProxy::GetResGroup()
{
	return pFactory->GetResGroup();
}

void TerrainS3DTextureBindingFactoryProxy::SetResGroup(uint32_t resGroup)
{
	pFactory->SetResGroup(resGroup);
}

uint32_t TerrainS3DTextureBindingFactoryProxy::GetFreePoolMax()
{
	return pFactory->GetFreePoolMax();
}

void TerrainS3DTextureBindingFactoryProxy::SetFreePoolMax(uint32_t freePoolMax)
{
	pFactory->SetFreePoolMax(freePoolMax);
}

void TerrainS3DTextureBindingFactoryProxy::FlushFreePool()
{
	pFactory->FlushFreePool();
}

void TerrainS3DTextureBindingFactoryProxy::FlushAll()
{
	pFactory->FlushAll();
}

void TerrainS3DTextureBindingFactoryProxy::GetStatsString(cIGZString& string)
{
	pFactory->GetStatsString(string);
}

void TerrainS3DTextureBindingFactoryProxy::FreeBinding(cS3DTextureBinding* pBinding)
{
	pFactory->FreeBinding(pBinding);
}
