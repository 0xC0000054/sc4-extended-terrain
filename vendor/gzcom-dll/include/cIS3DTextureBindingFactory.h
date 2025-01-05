#pragma once
#include "cIGZUnknown.h"

class cGZPersistResourceKey;
class cIGZGDriver;
class cIGZString;
class cS3DTextureBinding;
class cS3DTextureBindingInfo;
class cS3DTextureBufferInfo;

static const uint32_t GZCLSID_cIS3DTextureBindingFactory = 0x1ac0e11a;
static const uint32_t GZIID_cIS3DTextureBindingFactory = 0xfac0e219;

class cIS3DTextureBindingFactory : public cIGZUnknown
{
public:
	virtual bool SetGDriver(cIGZGDriver* pDriver) = 0;

	virtual bool GetBinding(cGZPersistResourceKey const& key, cS3DTextureBinding** ppBinding) = 0;
	virtual bool GetBinding(uint32_t iid, cS3DTextureBinding** ppBinding) = 0;
	virtual bool GetBinding(cS3DTextureBufferInfo* pInfo, cS3DTextureBinding** ppBinding) = 0;

	virtual void SetBindingImageData(cS3DTextureBinding* pBinding, cS3DTextureBufferInfo* pInfo) = 0;
	virtual void GetBindingInfo(cS3DTextureBinding* pBinding, cS3DTextureBindingInfo* pInfo) = 0;

	virtual void SetLodOffset(uint32_t offset) = 0;
	virtual void SetInternalTextureBitDepth(uint32_t bitDepth) = 0;

	virtual uint32_t GetResGroup() = 0;
	virtual void SetResGroup(uint32_t resGroup) = 0;

	virtual uint32_t GetFreePoolMax() = 0;
	virtual void SetFreePoolMax(uint32_t freePoolMax) = 0;

	virtual void FlushFreePool() = 0;
	virtual void FlushAll() = 0;

	virtual void GetStatsString(cIGZString& string) = 0;
	virtual void FreeBinding(cS3DTextureBinding* pTextureBinding) = 0;
};