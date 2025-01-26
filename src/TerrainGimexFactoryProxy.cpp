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

#include "TerrainGimexFactoryProxy.h"
#include "cGZPersistResourceKey.h"
#include "cIGZBuffer.h"
#include "cIGZCOM.h"
#include "cIGZFrameWork.h"
#include "cRZCOMDllDirector.h"
#include "Logger.h"

TerrainGimexFactoryProxy::TerrainGimexFactoryProxy(
	const ISettings& settings,
	const ITerrainTextureRedirectManager& textureRedirectManager)
	: settings(settings),
	  textureRedirectManager(textureRedirectManager)
{
	cIGZCOM* const pCOM = RZGetFramework()->GetCOMObject();

	pCOM->GetClassObject(GZCLSID_cIGZGimexFactory, GZIID_cIGZGimexFactory, factory.AsPPVoid());
}

bool TerrainGimexFactoryProxy::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_cIGZGimexFactory)
	{
		*ppvObj = static_cast<cIGZGimexFactory*>(this);
		AddRef();

		return true;
	}

	return cRZBaseUnknown::QueryInterface(riid, ppvObj);
}

uint32_t TerrainGimexFactoryProxy::AddRef()
{
	return cRZBaseUnknown::AddRef();
}

uint32_t TerrainGimexFactoryProxy::Release()
{
	return cRZBaseUnknown::Release();
}

bool TerrainGimexFactoryProxy::LoadFromFile(cIGZString const& path, cIGZBuffer* pBuffer, cGZBufferType bufferType)
{
	return factory->LoadFromFile(path, pBuffer, bufferType);
}

bool TerrainGimexFactoryProxy::LoadFromFile(cIGZString const& path, cIGZBuffer** outBuffer, cGZBufferType bufferType)
{
	return factory->LoadFromFile(path, outBuffer, bufferType);
}

bool TerrainGimexFactoryProxy::SaveToFile(
	cIGZString const& path,
	cIGZBuffer* pBuffer,
	SC4Rect<int32_t>& area,
	uint32_t formatID)
{
	return factory->SaveToFile(path, pBuffer, area, formatID);
}

bool TerrainGimexFactoryProxy::SaveToFile(cIGZString const& path, cIGZBuffer* pBuffer, uint32_t formatID)
{
	return factory->SaveToFile(path, pBuffer, formatID);
}

bool TerrainGimexFactoryProxy::LoadFromFile(cIGZFile* pFile, uint32_t gimexIndex, cIGZBuffer* pBuffer, cGZBufferType bufferType)
{
	return factory->LoadFromFile(pFile, gimexIndex, pBuffer, bufferType);
}

bool TerrainGimexFactoryProxy::CreateFromFile(
	cIGZFile* pFile,
	uint32_t gimexIndex,
	cIGZBuffer** outBuffer,
	cGZBufferType bufferType)
{
	return factory->CreateFromFile(pFile, gimexIndex, outBuffer, bufferType);
}

bool TerrainGimexFactoryProxy::LoadFromResource(
	cGZPersistResourceKey& key,
	uint32_t gimexIndex,
	cIGZBuffer* pBuffer,
	cGZBufferType bufferType)
{
	return factory->LoadFromResource(key, gimexIndex, pBuffer, bufferType);
}

bool TerrainGimexFactoryProxy::CreateFromResource(
	cGZPersistResourceKey& key,
	uint32_t gimexIndex,
	cIGZBuffer** outBuffer,
	cGZBufferType bufferType)
{
	cGZPersistResourceKey redirectedKey(
		key.type,
		key.group,
		textureRedirectManager.GetRedirectedTextureInstanceID(key.instance));

	if (settings.LogLoadedTextureIDs())
	{
		Logger::GetInstance().WriteLineFormatted(
			LogLevel::Info,
			"Requested texture 0x%08X,0x%08X,0x%08X",
			redirectedKey.type,
			redirectedKey.group,
			redirectedKey.instance);
	}

	bool result = factory->CreateFromResource(redirectedKey, gimexIndex, outBuffer, bufferType);

	if (!result && settings.LogTextureLoadErrors())
	{
		Logger::GetInstance().WriteLineFormatted(
			LogLevel::Info,
			"Failed to load texture 0x%08X,0x%08X,0x%08X",
			redirectedKey.type,
			redirectedKey.group,
			redirectedKey.instance);
	}

	return result;
}

bool TerrainGimexFactoryProxy::LoadFromSerialRecord(
	cIGZPersistDBSerialRecord* pRecord,
	uint32_t gimexIndex,
	cIGZBuffer* pBuffer,
	cGZBufferType bufferType)
{
	return factory->LoadFromSerialRecord(pRecord, gimexIndex, pBuffer, bufferType);
}

bool TerrainGimexFactoryProxy::CreateFromSerialRecord(
	cIGZPersistDBSerialRecord* pRecord,
	uint32_t gimexIndex,
	cIGZBuffer** outBuffer,
	cGZBufferType bufferType)
{
	return factory->CreateFromSerialRecord(pRecord, gimexIndex, outBuffer, bufferType);
}

bool TerrainGimexFactoryProxy::SaveToFile(
	cIGZFile* pFile,
	uint32_t gimexIndex,
	cIGZBuffer* pBuffer,
	SC4Rect<int32_t>& area,
	uint32_t unknown1)
{
	return factory->SaveToFile(pFile, gimexIndex, pBuffer, area, unknown1);
}

bool TerrainGimexFactoryProxy::SaveToFile(cIGZFile* pFile, uint32_t gimexIndex, cIGZBuffer* pBuffer, uint32_t unknown1)
{
	return factory->SaveToFile(pFile, gimexIndex, pBuffer, unknown1);
}

bool TerrainGimexFactoryProxy::SaveToResource(
	cGZPersistResourceKey& key,
	cIGZPersistDBSegment* pSegment,
	uint32_t gimexIndex,
	cIGZBuffer* pBuffer,
	SC4Rect<int32_t>& area,
	uint32_t formatID)
{
	return factory->SaveToResource(key, pSegment, gimexIndex, pBuffer, area, formatID);
}

bool TerrainGimexFactoryProxy::SaveToResource(
	cGZPersistResourceKey& key,
	cIGZPersistDBSegment* pSegment,
	uint32_t gimexIndex,
	cIGZBuffer* pBuffer,
	uint32_t formatID)
{
	return factory->SaveToResource(key, pSegment, gimexIndex, pBuffer, formatID);
}

bool TerrainGimexFactoryProxy::SaveToSerialRecord(
	cIGZPersistDBSerialRecord* pRecord,
	uint32_t gimexIndex,
	cIGZBuffer* pBuffer,
	uint32_t unknown1)
{
	return factory->SaveToSerialRecord(pRecord, gimexIndex, pBuffer, unknown1);
}

bool TerrainGimexFactoryProxy::SaveToSerialRecord(
	cIGZPersistDBSerialRecord* pRecord,
	uint32_t gimexIndex,
	cIGZBuffer* pBuffer,
	SC4Rect<int32_t>& area,
	uint32_t unknown1)
{
	return factory->SaveToSerialRecord(pRecord, gimexIndex, pBuffer, area, unknown1);
}
