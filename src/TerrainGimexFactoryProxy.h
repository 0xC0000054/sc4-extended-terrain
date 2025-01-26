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

#pragma once
#include "cIGZGimexFactory.h"
#include "cRZAutoRefCount.h"
#include "cRZBaseUnknown.h"
#include "ISettings.h"
#include "ITerrainTextureRedirectManager.h"

static constexpr uint32_t GZCLSID_TerrainGimexFactoryProxy = 0xE75D5221;

class TerrainGimexFactoryProxy : public cRZBaseUnknown, public cIGZGimexFactory
{
public:
	TerrainGimexFactoryProxy(
		const ISettings& settings,
		const ITerrainTextureRedirectManager& textureRedirectManager);

	// cRZBaseUnknown

	bool QueryInterface(uint32_t riid, void** ppvObj) override;
	uint32_t AddRef() override;
	uint32_t Release() override;

	// cIGZGimexFactory

	bool LoadFromFile(cIGZString const& path, cIGZBuffer* pBuffer, cGZBufferType bufferType) override;
	bool LoadFromFile(cIGZString const& path, cIGZBuffer** outBuffer, cGZBufferType bufferType) override;

	bool SaveToFile(cIGZString const& path, cIGZBuffer* pBuffer, SC4Rect<int32_t>& area, uint32_t formatID) override;
	bool SaveToFile(cIGZString const& path, cIGZBuffer* pBuffer, uint32_t formatID) override;
	bool LoadFromFile(cIGZFile* pFile, uint32_t gimexIndex, cIGZBuffer* pBuffer, cGZBufferType bufferType) override;
	bool CreateFromFile(cIGZFile* pFile, uint32_t gimexIndex, cIGZBuffer** outBuffer, cGZBufferType bufferType) override;

	bool LoadFromResource(cGZPersistResourceKey& key, uint32_t gimexIndex, cIGZBuffer* pBuffer, cGZBufferType bufferType) override;
	bool CreateFromResource(cGZPersistResourceKey& key, uint32_t gimexIndex, cIGZBuffer** outBuffer, cGZBufferType bufferType) override;
	bool LoadFromSerialRecord(cIGZPersistDBSerialRecord* pRecord, uint32_t gimexIndex, cIGZBuffer* pBuffer, cGZBufferType bufferType) override;
	bool CreateFromSerialRecord(cIGZPersistDBSerialRecord* pRecord, uint32_t gimexIndex, cIGZBuffer** outBuffer, cGZBufferType bufferType) override;

	bool SaveToFile(cIGZFile* pFile, uint32_t gimexIndex, cIGZBuffer* pBuffer, SC4Rect<int32_t>& area, uint32_t formatID) override;
	bool SaveToFile(cIGZFile* pFile, uint32_t gimexIndex, cIGZBuffer* pBuffer, uint32_t formatID) override;
	bool SaveToResource(cGZPersistResourceKey& key, cIGZPersistDBSegment* pSegment, uint32_t gimexIndex, cIGZBuffer* pBuffer, SC4Rect<int32_t>& area, uint32_t formatID) override;
	bool SaveToResource(cGZPersistResourceKey& key, cIGZPersistDBSegment* pSegment, uint32_t gimexIndex, cIGZBuffer* pBuffer, uint32_t formatID) override;
	bool SaveToSerialRecord(cIGZPersistDBSerialRecord* pRecord, uint32_t gimexIndex, cIGZBuffer* pBuffer, SC4Rect<int32_t>& area, uint32_t formatID) override;
	bool SaveToSerialRecord(cIGZPersistDBSerialRecord* pRecord, uint32_t gimexIndex, cIGZBuffer* pBuffer, uint32_t formatID) override;

private:
	const ISettings& settings;
	const ITerrainTextureRedirectManager& textureRedirectManager;
	cRZAutoRefCount<cIGZGimexFactory> factory;
};

