#include "cRZBaseSystemService.h"

cRZBaseSystemService::cRZBaseSystemService(uint32_t serviceID, int32_t servicePriority)
	: serviceID(serviceID),
	  servicePriority(servicePriority),
	  serviceTickPriority(servicePriority),
	  serviceRunning(false)
{
}

bool cRZBaseSystemService::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_cIGZSystemService)
	{
		*ppvObj = static_cast<cIGZSystemService*>(this);
		AddRef();

		return true;
	}

	return cRZBaseUnknown::QueryInterface(riid, ppvObj);
}

uint32_t cRZBaseSystemService::AddRef()
{
	return cRZBaseUnknown::AddRef();
}

uint32_t cRZBaseSystemService::Release()
{
	return cRZBaseUnknown::Release();
}

uint32_t cRZBaseSystemService::GetServiceID()
{
	return serviceID;
}

cIGZSystemService* cRZBaseSystemService::SetServiceID(uint32_t id)
{
	serviceID = id;
	return this;
}

int32_t cRZBaseSystemService::GetServicePriority()
{
	return servicePriority;
}

bool cRZBaseSystemService::IsServiceRunning()
{
	return serviceRunning;
}

cIGZSystemService* cRZBaseSystemService::SetServiceRunning(bool running)
{
	serviceRunning = running;
	return this;
}

bool cRZBaseSystemService::OnTick(uint32_t unknown1)
{
	return true;
}

bool cRZBaseSystemService::OnIdle(uint32_t unknown1)
{
	return true;
}

int32_t cRZBaseSystemService::GetServiceTickPriority()
{
	return serviceTickPriority;
}
