#include "QRCodeLoginCredentialFilter.h"
#include "Dll.h"
#include "guid.h"
#include <Objbase.h>


HRESULT QRCodeLoginCredentialFilter_CreateInstance(__in REFIID riid, __deref_out void** ppv)
{
	if (ppv == NULL)
		return E_INVALIDARG;

	*ppv = NULL;

	QRCodeLoginCredentialFilter *pProviderFilter = new QRCodeLoginCredentialFilter;
	if (pProviderFilter == NULL)
		return E_OUTOFMEMORY;

	HRESULT hr = pProviderFilter->QueryInterface(riid, ppv);

	pProviderFilter->Release();

	return hr;
}

QRCodeLoginCredentialFilter::QRCodeLoginCredentialFilter() :
	_cRef(1)
{
	DllAddRef();

}

QRCodeLoginCredentialFilter::~QRCodeLoginCredentialFilter()
{
	DllRelease();
}

STDMETHODIMP QRCodeLoginCredentialFilter::Filter(
	CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, DWORD dwFlags,
	GUID* rgclsidProviders, BOOL* rgbAllow, DWORD cProviders)
{
	switch (cpus)
	{
	case CPUS_LOGON:
	case CPUS_UNLOCK_WORKSTATION:
	{
		int i = 0;
		for (i = 0; i < (int)cProviders; i++)
		{
			TCHAR strCLSID[48];
			StringFromGUID2(rgclsidProviders[i], strCLSID, 48);
			FILE *f = NULL;
			fopen_s(&f,"C:\\1.txt", "at+");
			if (f != NULL) {
				fwprintf(f, L"%s\r\n", strCLSID);
				fclose(f);
			}
			if (IsEqualGUID(rgclsidProviders[i], CLSID_QRCodeLoginCredential))
				break;
		}
		if (i != (int)cProviders)
		{
			int j = 0;
			for (j = 0; j < (int)cProviders; j++)
				rgbAllow[j] = FALSE;
			rgbAllow[i] = TRUE;
		}
	}
	return S_OK;
	case CPUS_CREDUI:
	case CPUS_CHANGE_PASSWORD:
		return E_NOTIMPL;
	default:
		return E_INVALIDARG;
	}
}

HRESULT QRCodeLoginCredentialFilter::UpdateRemoteCredential(
	const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION* pcpcsIn
	, CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION* pcpcsOut)
{
	//return E_NOTIMPL;
	//Sleep(10000);
	
	HRESULT result;

	if (pcpcsIn != NULL && pcpcsIn->cbSerialization > 0
		&& (pcpcsOut->rgbSerialization = (BYTE *)CoTaskMemAlloc(pcpcsIn->cbSerialization)) != NULL)
	{
		pcpcsOut->ulAuthenticationPackage = pcpcsIn->ulAuthenticationPackage;
		pcpcsOut->clsidCredentialProvider = CLSID_QRCodeLoginCredential;
		pcpcsOut->cbSerialization = pcpcsIn->cbSerialization;
		CopyMemory(pcpcsOut->rgbSerialization, pcpcsIn->rgbSerialization, pcpcsIn->cbSerialization);
		result = S_OK;
	}
	else
		result = S_FALSE;

	FILE *f = NULL;
	fopen_s(&f, "C:\\2.txt", "at+");
	if (f != NULL) {
		fwprintf(f, L"CredentialProviderFilter::UpdateRemoteCredential(%p) returns %ld\r\n", pcpcsIn, result);
		fclose(f);
	}
	return result;
}
