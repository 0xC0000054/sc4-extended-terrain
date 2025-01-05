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
#include "cIS3DTextureBindingFactory.h"
#include "cRZAutoRefCount.h"
#include "cRZBaseSystemService.h"
#include "ISettings.h"
#include "ITerrainS3DTextureBindingFactoryProxy.h"

static constexpr uint32_t GZSRVID_TerrainS3DTextureBindingFactoryProxy = 0xF379B4BF;

class TerrainS3DTextureBindingFactoryProxy :
	public cRZBaseSystemService,
	public cIS3DTextureBindingFactory,
	public ITerrainS3DTextureBindingFactoryProxy
{
public:
	TerrainS3DTextureBindingFactoryProxy(const ISettings& settings);

	// ITerrainS3DTextureBindingFactoryProxy

	bool IsInitialized() const;

	// cRZBaseSystemService

	bool QueryInterface(uint32_t riid, void** ppvObj) override;
	uint32_t AddRef() override;
	uint32_t Release() override;

	bool Init() override;
	bool Shutdown() override;

	// cIS3DTextureBindingFactory

	bool SetGDriver(cIGZGDriver* pDriver) override;

	bool GetBinding(cGZPersistResourceKey const& key, cS3DTextureBinding** ppTextureBinding) override;
	bool GetBinding(uint32_t iid, cS3DTextureBinding** ppTextureBinding) override;
	bool GetBinding(cS3DTextureBufferInfo* pInfo, cS3DTextureBinding** ppTextureBinding) override;

	void SetBindingImageData(cS3DTextureBinding* pBinding, cS3DTextureBufferInfo* pInfo) override;
	void GetBindingInfo(cS3DTextureBinding* pBinding, cS3DTextureBindingInfo* pInfo) override;

	void SetLodOffset(uint32_t offset) override;
	void SetInternalTextureBitDepth(uint32_t bitDepth) override;

	uint32_t GetResGroup() override;
	void SetResGroup(uint32_t resGroup) override;

	uint32_t GetFreePoolMax() override;
	void SetFreePoolMax(uint32_t freePoolMax) override;

	void FlushFreePool() override;
	void FlushAll() override;

	void GetStatsString(cIGZString& string) override;
	void FreeBinding(cS3DTextureBinding* pTextureBinding) override;

private:
	const ISettings& settings;
	cIS3DTextureBindingFactory* pFactory;
};

