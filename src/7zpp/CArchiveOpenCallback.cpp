#include "stdafx.h"
#include "CArchiveOpenCallback.h"

STDMETHODIMP CArchiveOpenCallback::SetTotal(const UInt64* /* files */, const UInt64* /* bytes */)
{
    return S_OK;
}

STDMETHODIMP CArchiveOpenCallback::SetCompleted(const UInt64* /* files */, const UInt64* /* bytes */)
{
    return S_OK;
}

STDMETHODIMP CArchiveOpenCallback::CryptoGetTextPassword(BSTR* password)
{
    if (!PasswordIsDefined)   {
        // You can ask real password here from user
        // Password = GetPassword(OutStream);
        // PasswordIsDefined = true;
        PrintError("Password is not defined");
        return E_ABORT;
    }
    return StringToBstr(Password, password);
}