#pragma once
#include <credentialprovider.h>
#include <windows.h>
#include <strsafe.h>

#include "QRCodeLoginCredential.h"
#include "helpers.h"

class QRCodeLoginCredentialFilter : public ICredentialProviderFilter
{
public:
	// IUnknown
	IFACEMETHODIMP_(ULONG) AddRef()
	{
		return ++_cRef;
	}

	IFACEMETHODIMP_(ULONG) Release()
	{
		LONG cRef = --_cRef;
		if (!cRef)
		{
			delete this;
		}
		return cRef;
	}

	IFACEMETHODIMP QueryInterface(__in REFIID riid, __deref_out void** ppv)
	{
		static const QITAB qit[] =
		{
			QITABENT(QRCodeLoginCredentialFilter, ICredentialProviderFilter), // IID_ICredentialProvider
			{ 0 },
		};
		return QISearch(this, qit, riid, ppv);
	}

public:
	// ICredentialProviderFilter
	IFACEMETHODIMP Filter(
		CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, 
		DWORD dwFlags, 
		GUID* rgclsidProviders, 
		BOOL* rgbAllow, 
		DWORD cProviders);

	IFACEMETHODIMP UpdateRemoteCredential(
		const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION* pcpcsIn
		, CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION* pcpcsOut);


	friend HRESULT QRCodeLoginCredentialFilter_CreateInstance(__in REFIID riid, __deref_out void** ppv);

protected:
	QRCodeLoginCredentialFilter();
	__override ~QRCodeLoginCredentialFilter();

private:

private:
	LONG                                    _cRef;            // Used for reference counting.
};
