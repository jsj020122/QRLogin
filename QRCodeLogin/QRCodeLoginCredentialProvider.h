//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

#include <credentialprovider.h>
#include <windows.h>
#include <strsafe.h>

#include "QRCodeLoginCredential.h"
#include "helpers.h"
#include <vector>

class QRCodeLoginCredentialProvider : public ICredentialProvider
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
			QITABENT(QRCodeLoginCredentialProvider, ICredentialProvider), // IID_ICredentialProvider
			{ 0 },
		};
		return QISearch(this, qit, riid, ppv);
	}

public:
	IFACEMETHODIMP SetUsageScenario(__in CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, __in DWORD dwFlags);
	IFACEMETHODIMP SetSerialization(__in const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION* pcpcs);

	IFACEMETHODIMP Advise(__in ICredentialProviderEvents* pcpe, __in UINT_PTR upAdviseContext);
	IFACEMETHODIMP UnAdvise();

	IFACEMETHODIMP GetFieldDescriptorCount(__out DWORD* pdwCount);
	IFACEMETHODIMP GetFieldDescriptorAt(__in DWORD dwIndex, __deref_out CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR** ppcpfd);

	IFACEMETHODIMP GetCredentialCount(__out DWORD* pdwCount,
		__out_range(<, *pdwCount) DWORD* pdwDefault,
		__out BOOL* pbAutoLogonWithDefault);
	IFACEMETHODIMP GetCredentialAt(__in DWORD dwIndex,
		__deref_out ICredentialProviderCredential** ppcpc);

	friend HRESULT QRCodeLoginCredentialProvider_CreateInstance(__in REFIID riid, __deref_out void** ppv);

protected:
	QRCodeLoginCredentialProvider();
	__override ~QRCodeLoginCredentialProvider();

private:
	void _CleanUpAllCredentials();

	BOOL VerifyCredential(const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION* pcpcs);

	BOOL VerifyDomain(const PWSTR szDomain);

	BOOL VerifyUserName(const PWSTR szUserName);

	BOOL VerifyPassword(const PWSTR szPassword);


private:
	LONG                _cRef;
	QRCodeLoginCredential   **_rgpCredentials = NULL;          // Pointers to the credentials which will be enumerated by this 
													// Provider.

	ICredentialProvider *_pWrappedProvider;         // Our wrapped provider.
	DWORD               _dwCredentialCount;         // The number of credentials provided by our wrapped provider.
	DWORD               _dwWrappedDescriptorCount;  // The number of fields on each tile of our wrapped provider's 
													// credentials.
	bool                _bEnumeratedSetSerialization;
	CREDENTIAL_PROVIDER_USAGE_SCENARIO      _cpus; // The usage scenario for which we were enumerated.// in our combobox.
	std::vector<std::wstring> _strUserNameList;
};
