/*
 * This file is part of sc4-extended-terrain, a DLL Plugin for SimCity 4
 * that adds support for more terrain types.
 *
 * Copyright (C) 2024, 2025 Nicholas Hayes
 *
 * sc4-extended-terrain is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * sc4-extended-terrain is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with sc4-extended-terrain.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include "cIS3DTextureBindingFactory.h"
#include "cRZAutoRefCount.h"
#include "cRZBaseSystemService.h"
#include "cRZBaseUnknown.h"
#include "ISettings.h"
#include "ITerrainS3DTextureBindingFactoryProxy.h"
#include "ITerrainTextureRedirectManager.h"

static constexpr uint32_t GZSRVID_TerrainS3DTextureBindingFactoryProxy = 0xF379B4BF;

class TerrainS3DTextureBindingFactoryProxy :
	public cRZBaseUnknown,
	public cRZBaseSystemService,
	public cIS3DTextureBindingFactory,
	public ITerrainS3DTextureBindingFactoryProxy
{
public:
	TerrainS3DTextureBindingFactoryProxy(
		const ISettings& settings,
		const ITerrainTextureRedirectManager& textureRedirectManager);

	// ITerrainS3DTextureBindingFactoryProxy

	bool IsInitialized() const;

	// cRZBaseUnknown

	bool QueryInterface(uint32_t riid, void** ppvObj) override;
	uint32_t AddRef() override;
	uint32_t Release() override;

	// cRZBaseSystemService

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
	const ITerrainTextureRedirectManager& textureRedirectManager;
	cIS3DTextureBindingFactory* pFactory;
};

